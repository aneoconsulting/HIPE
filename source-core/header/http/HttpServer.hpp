//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#ifndef SERVER_HTTP_HPP
#define	SERVER_HTTP_HPP

#pragma warning(push, 0)
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#pragma warning(pop)

#include <unordered_map>
#include <thread>
#include <functional>
#include <iostream>
#include <sstream>

namespace http
{
    template <class socket_type>
    class Server;
    
    template <class socket_type>
    class ServerBase {
    public:
        virtual ~ServerBase() {}

        class Response : public std::ostream {
            friend class ServerBase<socket_type>;

            boost::asio::streambuf streambuf;

            std::shared_ptr<socket_type> socket;

            Response(const std::shared_ptr<socket_type> &socket): std::ostream(&streambuf), socket(socket) {}

        public:
            size_t size() {
                return streambuf.size();
            }
        };
        
        class Content : public std::istream {
            friend class ServerBase<socket_type>;
        public:
            size_t size() {
                return streambuf.size();
            }
            std::string string() {
                std::stringstream ss;
                ss << rdbuf();
                return ss.str();
            }
        public:
            boost::asio::streambuf &streambuf;
            Content(boost::asio::streambuf &streambuf): std::istream(&streambuf), streambuf(streambuf) {}
        };
        
		class Request {
			friend class ServerBase < socket_type > ;
			friend class Server < socket_type > ;
		public:
			//Based on http://www.boost.org/doc/libs/1_60_0/doc/html/unordered/hash_equality.html
			class iequal_to {
			public:
				bool operator()(const std::string &key1, const std::string &key2) const {
					return boost::algorithm::iequals(key1, key2);
				}
			};
			class ihash {
			public:
				size_t operator()(const std::string &key) const {
					std::size_t seed = 0;
					for (char& c : str) 
						boost::hash_combine(seed, std::tolower(c));
					return seed;
				}
			};
		public:
			std::string method, path, http_version;

			Content content;

			std::unordered_multimap<std::string, std::string, ihash, iequal_to> header;

			REGEX_NS::smatch path_match;

			std::string remote_endpoint_address;
			unsigned short remote_endpoint_port;

		public:
			Request(const socket_type &socket) : content(streambuf) {
				try {
					remote_endpoint_address = socket.lowest_layer().remote_endpoint().address().to_string();
					remote_endpoint_port = socket.lowest_layer().remote_endpoint().port();
				}
				catch (...) {}
			}

			boost::asio::streambuf streambuf;
		};
        
        class Config {
            friend class ServerBase<socket_type>;

            Config(unsigned short port, size_t num_threads): num_threads(num_threads), port(port), reuse_address(true) {}
            size_t num_threads;
        public:
            unsigned short port;
            ///IPv4 address in dotted decimal form or IPv6 address in hexadecimal notation.
            ///If empty, the address will be any address.
            std::string address;
            ///Set to false to avoid binding the socket to an address that is already in use.
            bool reuse_address;
        };
        ///Set before calling start().
        Config config;
        
        std::unordered_map<std::string, std::unordered_map<std::string, 
            std::function<void(std::shared_ptr<typename ServerBase<socket_type>::Response>, std::shared_ptr<typename ServerBase<socket_type>::Request>)> > >  resource;
        
        std::unordered_map<std::string, 
            std::function<void(std::shared_ptr<typename ServerBase<socket_type>::Response>, std::shared_ptr<typename ServerBase<socket_type>::Request>)> > default_resource;
        
        std::function<void(std::shared_ptr<typename ServerBase<socket_type>::Request>, const boost::system::error_code&)> on_error;

    private:
        std::vector<std::pair<std::string, std::vector<std::pair<REGEX_NS::regex,
            std::function<void(std::shared_ptr<typename ServerBase<socket_type>::Response>, std::shared_ptr<typename ServerBase<socket_type>::Request>)> > > > > opt_resource;
        
    public:
        virtual void start() {
            //Copy the resources to opt_resource for more efficient request processing
            opt_resource.clear();
            for(auto& res: resource) {
                for(auto& res_method: res.second) {
                    auto it=opt_resource.end();
                    for(auto opt_it=opt_resource.begin();opt_it!=opt_resource.end();opt_it++) {
                        if(res_method.first==opt_it->first) {
                            it=opt_it;
                            break;
                        }
                    }
                    if(it==opt_resource.end()) {
                        opt_resource.emplace_back();
                        it=opt_resource.begin()+(opt_resource.size()-1);
                        it->first=res_method.first;
                    }
                    it->second.emplace_back(REGEX_NS::regex(res.first), res_method.second);
                }
            }

            if(!io_service)
                io_service=std::make_shared<boost::asio::io_service>();

            if(io_service->stopped())
                io_service->reset();

            boost::asio::ip::tcp::endpoint endpoint;
            if(config.address.size()>0)
                endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(config.address), config.port);
            else
                endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), config.port);
            
            if(!acceptor)
                acceptor=std::unique_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(*io_service));
            acceptor->open(endpoint.protocol());
            acceptor->set_option(boost::asio::socket_base::reuse_address(config.reuse_address));
            acceptor->bind(endpoint);
            acceptor->listen();
     
            accept(); 
            
            //If num_threads>1, start m_io_service.run() in (num_threads-1) threads for thread-pooling
            threads.clear();
            for(size_t c=1;c<config.num_threads;c++) {
                threads.emplace_back([this](){
                    io_service->run();
                });
            }

            //Main thread
            if(config.num_threads>0)
                io_service->run();

            //Wait for the rest of the threads, if any, to finish as well
            for(auto& t: threads) {
                t.join();
            }
        }
        
        void stop() {
            acceptor->close();
            if(config.num_threads>0)
                io_service->stop();
        }
        
        ///Use this function if you need to recursively send parts of a longer message
        void send(const std::shared_ptr<Response> &response, const std::function<void(const boost::system::error_code&)>& callback=nullptr) const {
            boost::asio::async_write(*response->socket, response->streambuf, [this, response, callback](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
                if(callback)
                    callback(ec);
            });
        }

        /// If you have your own boost::asio::io_service, store its pointer here before running start().
        /// You might also want to set config.num_threads to 0.
        std::shared_ptr<boost::asio::io_service> io_service;
    protected:
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
        std::vector<std::thread> threads;
        
        long timeout_request;
        long timeout_content;
        
        ServerBase(unsigned short port, size_t num_threads, long timeout_request, long timeout_send_or_receive) :
                config(port, num_threads), timeout_request(timeout_request), timeout_content(timeout_send_or_receive) {}
        
        virtual void accept()=0;
        
        std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(const std::shared_ptr<socket_type> &socket, long seconds) {
            if(seconds==0)
                return nullptr;
            
            auto timer=std::make_shared<boost::asio::deadline_timer>(*io_service);
            timer->expires_from_now(boost::posix_time::seconds(seconds));
            timer->async_wait([socket](const boost::system::error_code& ec){
                if(!ec) {
                    boost::system::error_code ec;
                    socket->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                    socket->lowest_layer().close();
                }
            });
            return timer;
        }
        
        void read_request_and_content(const std::shared_ptr<socket_type> &socket) {
            //Create new streambuf (Request::streambuf) for async_read_until()
            //shared_ptr is used to pass temporary objects to the asynchronous functions
            std::shared_ptr<Request> request(new Request(*socket));

            //Set timeout on the following boost::asio::async-read or write function
            auto timer=this->get_timeout_timer(socket, timeout_request);
                        
            boost::asio::async_read_until(*socket, request->streambuf, "\r\n\r\n",
                    [this, socket, request, timer](const boost::system::error_code& ec, size_t bytes_transferred) {
                if(timer)
                    timer->cancel();
                if(!ec) {
                    //request->streambuf.size() is not necessarily the same as bytes_transferred, from Boost-docs:
                    //"After a successful async_read_until operation, the streambuf may contain additional data beyond the delimiter"
                    //The chosen solution is to extract lines from the stream directly when parsing the header. What is left of the
                    //streambuf (maybe some bytes of the content) is appended to in the async_read-function below (for retrieving content).
                    size_t num_additional_bytes=request->streambuf.size()-bytes_transferred;
                    
                    if(!this->parse_request(request))
                        return;
                    
                    //If content, read that as well
                    auto it=request->header.find("Content-Length");
                    if(it!=request->header.end()) {
                        unsigned long long content_length;
                        try {
                            content_length=stoull(it->second);
                        }
                        catch(const std::exception &e) {
                            if(on_error)
                                on_error(request, boost::system::error_code(boost::system::errc::protocol_error, boost::system::generic_category()));
                            return;
                        }
                        if(content_length>num_additional_bytes) {
                            //Set timeout on the following boost::asio::async-read or write function
                            auto timer=this->get_timeout_timer(socket, timeout_content);
                            boost::asio::async_read(*socket, request->streambuf,
                                    boost::asio::transfer_exactly(content_length-num_additional_bytes),
                                    [this, socket, request, timer]
                                    (const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
                                if(timer)
                                    timer->cancel();
                                if(!ec)
                                    this->find_resource(socket, request);
                                else if(on_error)
                                    on_error(request, ec);
                            });
                        }
                        else
                            this->find_resource(socket, request);
                    }
                    else
                        this->find_resource(socket, request);
                }
                else if(on_error)
                    on_error(request, ec);
            });
        }

        bool parse_request(const std::shared_ptr<Request> &request) const {
            std::string line;
            getline(request->content, line);
            size_t method_end;
            if((method_end=line.find(' '))!=std::string::npos) {
                size_t path_end;
                if((path_end=line.find(' ', method_end+1))!=std::string::npos) {
                    request->method=line.substr(0, method_end);
                    request->path=line.substr(method_end+1, path_end-method_end-1);

                    size_t protocol_end;
                    if((protocol_end=line.find('/', path_end+1))!=std::string::npos) {
                        if(line.compare(path_end+1, protocol_end-path_end-1, "HTTP")!=0)
                            return false;
                        request->http_version=line.substr(protocol_end+1, line.size()-protocol_end-2);
                    }
                    else
                        return false;

                    getline(request->content, line);
                    size_t param_end;
                    while((param_end=line.find(':'))!=std::string::npos) {
                        size_t value_start=param_end+1;
                        if((value_start)<line.size()) {
                            if(line[value_start]==' ')
                                value_start++;
                            if(value_start<line.size())
                                request->header.insert(std::make_pair(line.substr(0, param_end), line.substr(value_start, line.size()-value_start-1)));
                        }
    
                        getline(request->content, line);
                    }
                }
                else
                    return false;
            }
            else
                return false;
            return true;
        }

        void find_resource(const std::shared_ptr<socket_type> &socket, const std::shared_ptr<Request> &request) {
            //Find path- and method-match, and call write_response
            for(auto& res: opt_resource) {
                if(request->method==res.first) {
                    for(auto& res_path: res.second) {
                        REGEX_NS::smatch sm_res;
                        if(REGEX_NS::regex_match(request->path, sm_res, res_path.first)) {
                            request->path_match=std::move(sm_res);
                            write_response(socket, request, res_path.second);
                            return;
                        }
                    }
                }
            }
            auto it_method=default_resource.find(request->method);
            if(it_method!=default_resource.end()) {
                write_response(socket, request, it_method->second);
            }
        }
        
        void write_response(const std::shared_ptr<socket_type> &socket, const std::shared_ptr<Request> &request, 
                std::function<void(std::shared_ptr<typename ServerBase<socket_type>::Response>,
                                   std::shared_ptr<typename ServerBase<socket_type>::Request>)>& resource_function) {
            //Set timeout on the following boost::asio::async-read or write function
            auto timer=this->get_timeout_timer(socket, timeout_content);

            auto response=std::shared_ptr<Response>(new Response(socket), [this, request, timer](Response *response_ptr) {
                auto response=std::shared_ptr<Response>(response_ptr);
                this->send(response, [this, response, request, timer](const boost::system::error_code& ec) {
                    if(timer)
                        timer->cancel();
                    if(!ec) {
                        float http_version;
                        try {
                            http_version=stof(request->http_version);
                        }
                        catch(const std::exception &e){
                            if(on_error)
                                on_error(request, boost::system::error_code(boost::system::errc::protocol_error, boost::system::generic_category()));
                            return;
                        }
                        
                        auto range=request->header.equal_range("Connection");
                        for(auto it=range.first;it!=range.second;it++) {
                            if(boost::iequals(it->second, "close"))
                                return;
                        }
                        if(http_version>1.05)
                            this->read_request_and_content(response->socket);
                    }
                    else if(on_error)
                        on_error(request, ec);
                });
            });

            try {
                resource_function(response, request);
            }
            catch(const std::exception &e) {
                if(on_error)
                    on_error(request, boost::system::error_code(boost::system::errc::operation_canceled, boost::system::generic_category()));
                return;
            }
        }
    };
    
    template<class socket_type>
    class Server : public ServerBase<socket_type> {};
    
    typedef boost::asio::ip::tcp::socket HTTP;
    
    template<>
    class Server<HTTP> : public ServerBase<HTTP> {
    public:
        Server(unsigned short port, size_t num_threads=1, long timeout_request=5, long timeout_content=300) :
                ServerBase<HTTP>::ServerBase(port, num_threads, timeout_request, timeout_content) {}
        
    protected:
        void accept() {
            //Create new socket for this connection
            //Shared_ptr is used to pass temporary objects to the asynchronous functions
            auto socket=std::make_shared<HTTP>(*io_service);
                        
            acceptor->async_accept(*socket, [this, socket](const boost::system::error_code& ec){
                //Immediately start accepting a new connection (if io_service hasn't been stopped)
                if (ec != boost::asio::error::operation_aborted)
                    accept();
                                
                if(!ec) {
                    boost::asio::ip::tcp::no_delay option(true);
                    socket->set_option(option);
                    
                    this->read_request_and_content(socket);
                }
                else if(on_error)
                    on_error(std::shared_ptr<Request>(new Request(*socket)), ec);
            });
        }
    };
}
#endif	/* SERVER_HTTP_HPP */
