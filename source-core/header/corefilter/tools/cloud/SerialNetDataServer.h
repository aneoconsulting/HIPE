//%HIPE_LICENSE%
#pragma once
#pragma warning(push, 0) 
#include <opencv2/core/mat.hpp>

#include <atomic>
#if defined(WIN32) && defined(ZLIB_RELEASE)
#define BOOST_ZLIB_BINARY zlib
#elif defined(WIN32) && defined(ZLIB_DEBUG)
#define BOOST_ZLIB_BINARY zlibd
#endif
#include <boost/asio.hpp>

#include <boost/bind.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#pragma warning(pop) 
#include <core/HipeStatus.h>
#include <core/queue/ConcurrentQueue.h>


#include <boost/serialization/vector.hpp>
#include <core/HipeException.h>
#include <corefilter/filter_export.h>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace boost {
	namespace serialization {

		template<class Archive>
		void serialize(Archive &ar, cv::Mat& mat, const unsigned int)
		{
			int cols, rows, type;
			bool continuous;

			if (Archive::is_saving::value) {
				cols = mat.cols; rows = mat.rows; type = mat.type();
				continuous = mat.isContinuous();
			}

			ar & cols & rows & type & continuous;

			if (Archive::is_loading::value)
				mat.create(rows, cols, type);

			if (continuous) {
				const unsigned int data_size = rows * cols * mat.elemSize();
				ar & boost::serialization::make_array(mat.ptr(), data_size);
			}
			else {
				const unsigned int row_size = cols*mat.elemSize();
				for (int i = 0; i < rows; i++) {
					ar & boost::serialization::make_array(mat.ptr(i), row_size);
				}
			}

		}

	}
}

/// The connection class provides serialization primitives on top of a socket.
/**
* Each message sent using this class consists of:
* @li An 8-byte header containing the length of the serialized data in
* hexadecimal.
* @li The serialized data.
*/
class Connection
{
	std::shared_ptr<boost::asio::io_service> iservice;
public:
	/// Constructor.
	Connection(std::shared_ptr<boost::asio::io_service> ioservice)
		
	{
		inbound_data_.resize(1);
		iservice = ioservice;
		socket_ = std::make_shared<boost::asio::ip::tcp::socket>(*ioservice);
		mem = nullptr;
		size_mem = 0;
	}

	~Connection()
	{
		if (mem != nullptr) free(mem);
	}

	/// Get the underlying socket. Used for making a connection or for accepting
	/// an incoming connection.
	std::shared_ptr<boost::asio::ip::tcp::socket> & socket()
	{
		return socket_;
	}

	char *mem;
	size_t size_mem;

	/// Asynchronously write a data structure to the socket.
	template <typename Handler>
	void async_write(const cv::Mat & mat, Handler handler)
	{
		// Serialize the data first so we know how large it is.
		std::string serial_str;
		std::stringstream os(std::ios_base::binary| std::ios_base::out| std::ios_base::in);

		{ // use scope to ensure archive and filtering stream buffer go out of scope before stream
			boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
			out.push(boost::iostreams::zlib_compressor(boost::iostreams::zlib::best_speed));
			out.push(os);

			boost::archive::binary_oarchive oa(out, boost::archive::no_header);
			oa << mat;

			//os.flush();
		}
		if (mem==nullptr) {
			mem = (char *)malloc(os.str().length());
			size_mem = os.str().length();
		}

		if (size_mem < os.str().length())
		{
			if (mem != nullptr) { free(mem); mem = nullptr; }
			mem = (char *)malloc(os.str().length());
			size_mem = os.str().length();
		}

		memcpy(mem, os.str().data(), os.str().length());

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::setw(header_length)
			<< std::hex << os.str().length();
		if (!header_stream || header_stream.str().size() != header_length)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			throw HipeException("Somethings is groing wrong with the data size in header");
		}
		outbound_header_ = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header_));
		buffers.push_back(boost::asio::buffer(mem, os.str().length()));
		boost::asio::write(*socket_, buffers);
	}

	/// Asynchronously write a data structure to the socket.
	template <typename Handler>
	void async_write(const std::string text, Handler handler)
	{
		// Serialize the data first so we know how large it is.
		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive oa(s, boost::archive::no_header);

		oa << text;
	

		s.flush();
		outbound_data_ = serial_str;

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::setw(header_length)
			<< std::hex << outbound_data_.size();
		if (!header_stream || header_stream.str().size() != header_length)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			throw HipeException("Somethings is groing wrong with the data size in header");
		}
		outbound_header_ = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header_));
		buffers.push_back(boost::asio::buffer(outbound_data_));
		boost::asio::write(*socket_, buffers);
	}


	/// Asynchronously read a data structure from the socket.
	HipeStatus read(cv::Mat & mat)
	{
		try
		{
			// Issue a read operation to read exactly the number of bytes in a header.
			boost::asio::read(*socket_, boost::asio::buffer(inbound_header_));
			std::istringstream is(std::string(inbound_header_, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				throw HipeException("Fail to read data from slave");
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			boost::asio::read(*socket_, boost::asio::buffer(inbound_data_));
			std::stringstream isData(std::string(inbound_data_.data(), inbound_data_.data()+inbound_data_.size()), std::ios_base::binary| std::ios_base::out | std::ios_base::in);
			std::string serial_str;
			
			 boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			 in.push(boost::iostreams::zlib_decompressor());
			 in.push(isData);
			boost::archive::binary_iarchive ia(in, boost::archive::no_header);

			try 
			{
				ia >> mat;
			} 
			catch (const boost::archive::archive_exception &e) 
			{
					if (e.code != boost::archive::archive_exception::input_stream_error) {
						throw HipeException("Fail to decompress Matrix");
					}
			}
			

		}
		catch (boost::system::system_error & ex)
		{
			std::stringstream error;

			error << "Reading data is failing check inner exception : \n" << ex.what() << std::endl;
			std::cerr << error.str();
			throw HipeException(error.str());
		}
		catch (std::exception & ex)
		{
			std::stringstream error;

			error << "Reading data is failing check inner exception : \n" << ex.what() << std::endl;
			std::cerr << error.str();
			throw HipeException(error.str());
		}

		return OK;
	}


	HipeStatus read(std::string & text)
	{
		try
		{
			// Issue a read operation to read exactly the number of bytes in a header.
			boost::asio::read(*socket_, boost::asio::buffer(inbound_header_));
			std::istringstream is(std::string(inbound_header_, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				throw HipeException("Fail to read data from slave");
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			boost::asio::read(*socket_, boost::asio::buffer(inbound_data_));
			std::string serial_str;
			// wrap buffer inside a stream and deserialize serial_str into obj
			boost::iostreams::basic_array_source<char> device(inbound_data_.data(), inbound_data_.size());
			boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
			boost::archive::binary_iarchive ia(s, boost::archive::no_header);
			ia >> text;

		}
		catch (boost::system::system_error & ex)
		{
			std::stringstream error;

			error << "Reading data is failing check inner exception : \n" << ex.what() << std::endl;
			std::cerr << error.str();
			throw HipeException(error.str());
		}
		catch (std::exception & ex)
		{
			std::stringstream error;

			error << "Reading data is failing check inner exception : \n" << ex.what() << std::endl;
			std::cerr << error.str();
			throw HipeException(error.str());
		}

		return OK;
	}


	/// Asynchronously read a data structure from the socket.
	template <typename T, typename Handler>
	void async_read(T& t, Handler handler)
	{
		// Issue a read operation to read exactly the number of bytes in a header.
		void (Connection::*f)(
			const boost::system::error_code&,
			T&, boost::tuple<Handler>)
			= &Connection::handle_read_header<T, Handler>;
		boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
			boost::bind(f,
				this, boost::asio::placeholders::error, boost::ref(t),
				boost::make_tuple(handler)));
	}

	/// Handle a completed read of a message header. The handler is passed using
	/// a tuple since boost::bind seems to have trouble binding a function object
	/// created using boost::bind as a parameter.
	template <typename T, typename Handler>
	void handle_read_header(const boost::system::error_code& e,
		T& t, boost::tuple<Handler> handler)
	{
		if (e)
		{
			boost::get<0>(handler)(e);
		}
		else
		{
			// Determine the length of the serialized data.
			std::istringstream is(std::string(inbound_header_, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			void (Connection::*f)(
				const boost::system::error_code&,
				T&, boost::tuple<Handler>)
				= &Connection::handle_read_data<T, Handler>;
			boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
				boost::bind(f, this,
					boost::asio::placeholders::error, boost::ref(t), handler));
		}
	}

	/// Handle a completed read of message data.
	template <typename T, typename Handler>
	void handle_read_data(const boost::system::error_code& e,
		T& t, boost::tuple<Handler> handler)
	{
		if (e)
		{
			boost::get<0>(handler)(e);
		}
		else
		{
			// Extract the data structure from the data just received.
			try
			{
				std::string archive_data(&inbound_data_[0], inbound_data_.size());
				std::istringstream archive_stream(archive_data);
				boost::archive::text_iarchive archive(archive_stream);
				archive >> t;
			}
			catch (std::exception& e)
			{
				// Unable to decode data.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			boost::get<0>(handler)(e);
		}
	}

private:
	/// The underlying socket.
	std::shared_ptr<boost::asio::ip::tcp::socket> socket_;


	/// The size of a fixed length header.
	enum { header_length = 8 };

	/// Holds an outbound header.
	std::string outbound_header_;

	/// Holds the outbound data.
	std::string outbound_data_;

	/// Holds an inbound header.
	char inbound_header_[header_length];

	/// Holds the inbound data.
	std::vector<char> inbound_data_;
};


class FILTER_EXPORT HipeConnection
{
	std::shared_ptr<Connection> _impl_connector;
	std::shared_ptr<boost::asio::io_service> ioservice;

public:
	HipeConnection()
	{
			
	}

	HipeConnection(std::shared_ptr<boost::asio::io_service> ioservice);

	~HipeConnection();
	
	HipeStatus Cancel();

	HipeStatus Close();

	std::shared_ptr<boost::asio::ip::tcp::socket>& socket();

	static void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred);

	void send(const cv::Mat& mat) const;

	void send(const std::string& text) const;

	HipeStatus read(cv::Mat& mat) const;
	HipeStatus read(std::string& text) const;
};

class FILTER_EXPORT SerialNetDataServer
{
	int _bufferingSize;
	std::shared_ptr<HipeConnection> connector;
	std::shared_ptr<boost::asio::io_service> ioservice;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
	typedef boost::asio::ip::tcp::socket TCP;
	

	std::atomic<bool> a_isActive;
public:
	typedef void(SerialNetDataServer::* ptr_func)();

	std::atomic<bool> & isActive()
	{
		return a_isActive;
	}

private:
	
public:
	core::queue::ConcurrentQueue<cv::Mat> imagesStack;
	core::queue::ConcurrentQueue<std::string> stringStack;

	std::unique_ptr<boost::thread> thr_server;

	SerialNetDataServer() : _bufferingSize(1)
	{
		a_isActive = false;
	}

	SerialNetDataServer(int port, int bufferingSize = 1) : _bufferingSize(bufferingSize)
	{
		a_isActive = false;
	}

	SerialNetDataServer(const SerialNetDataServer & right) : _bufferingSize(right._bufferingSize), connector(right.connector)
	{
		a_isActive = right.a_isActive.load();
	}

	void operator=(const SerialNetDataServer & right) 
	{
		_bufferingSize = (right._bufferingSize);
		connector = (right.connector);

		a_isActive = right.a_isActive.load();
		ioservice = right.ioservice;
	}

	void ImagesHandler();

	void TextHandler();
	void TextReceiverHandler();

	#ifdef WIN32
	void accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::_Binder<std::_Unforced, void( SerialNetDataServer::*&)(), SerialNetDataServer*> binder);
		void startServer(int port, std::_Binder<std::_Unforced, void( SerialNetDataServer::*&)(), SerialNetDataServer*> binder);

	#else
		void accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::function<void()> binder);
		void startServer(int port, std::function<void()> binder);
			
	#endif


	void send(const cv::Mat image) const;
	void send(const std::string& text) const;

	void read(cv::Mat& image) const;

	void stop();;
};

