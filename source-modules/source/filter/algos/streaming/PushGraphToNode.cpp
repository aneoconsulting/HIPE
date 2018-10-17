
#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#pragma warning(pop)



#include <algos/streaming/PushGraphToNode.h>
#include <algos/streaming/PopGraphToNode.h>
#include <data/FileVideoInput.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>
#include <http/HttpClient.h>
#include <regex>
#include <stack>

std::string filter::algos::PushGraphToNode::rebuildJsonFromGraph()
{
	json::JsonTree treeGraph;
	json::JsonTree filterList;
	std::string dataSourceName = std::string("Slave_request_dataSource");
	
	RegisterTable & register_table = RegisterTable::getInstance();
	std::stack<filter::Model*> heap;
	
	filter::Model *parent = this;

	//Parent is the PushGrahpNode nothing to do with it
	heap.push(parent);

	std::map<std::string, filter::Model *> childCache; //To avoid multiple instance of same object with the same name;
	bool alreadyFound = false;
	int level_popGraph = 0;
	int level_max_node = 0;
	json::JsonTree filterChild;
	

	filterChild.put(std::string("name"), std::string("Slave_request"));
	while (!heap.empty())
	{
		parent = heap.top();
		heap.pop();

		
		for (auto childMap : parent->getChildrens())
		{
			filter::Model* child = nullptr;
			if (childCache.find(childMap.second->getName()) != childCache.end())
			{
				child = childCache[childMap.second->getName()];
				if (child->getConstructorName().find("PopGraphToNode") != std::string::npos && alreadyFound == true)
				{
					throw HipeException("The exported graph must have only one PopToGrapheNode");
				}
				continue;
			}
			else
			{
				child = static_cast<filter::Model*>((childMap.second));
				childCache[childMap.second->getName()] = child;

			}
			
			
			//If we havec reached the filter PopGraphToNode it's the end
			if (child->getConstructorName().find("PopGraphToNode") != std::string::npos && alreadyFound == false)
			{
				//Prepare PopGraphNode to receive data from slave address 
				PopGraphToNode* poper = static_cast<PopGraphToNode *>(child);
				std::stringstream built_receiver_server;
				built_receiver_server << std::string(host_or_ip) << ":" << std::to_string(3101);
				std::string str = built_receiver_server.str();
				poper->set_address(str);


				alreadyFound = true;
				level_popGraph = child->getLevel();

				json::JsonTree currentFilter;

				//Write name of filter
				std::stringstream build_name;
				//build_name << 
				currentFilter.put("name", std::string("SerialNetSlaveSender__") + child->getName());

				//Write field of filter
				currentFilter.put("host_or_ip", local_adress + ":" + std::to_string(3101));

				//Write dependencies
				json::JsonTree containerNeed;
				for (auto & parentFilter : child->getParents())
				{
					json::JsonTree deps;
					if (parent->getConstructorName().find(getConstructorName()) != std::string::npos)
					{
						deps.put(std::string(""), dataSourceName);
					}
					else
						deps.put(std::string(""), parent->getName());

					containerNeed.push_back("", deps);
				}

				currentFilter.add_child("need", containerNeed);
				json::JsonTree containerFilter;
				
				containerFilter.add_child("SerialNetSlaveSender", currentFilter);
				filterList.push_back("", containerFilter);
			}

			else 
			{
				//Task for current node
				auto field_names = register_table.getVarNames(child->getConstructorName());
				json::JsonTree currentFilter;
				

				//Write name of filter
				currentFilter.put("name", child->getName());
				
				//Write field of filter
				for (auto &fieldname : field_names)
				{
					register_table.getVariable(currentFilter, child, fieldname);
				}

				//Write dependencies
				json::JsonTree containerNeed;
				for (auto & parentFilter : child->getParents())
				{
					json::JsonTree deps;

					Model* localParent = parentFilter.second;

					if (localParent->getConstructorName().find(getConstructorName()) != std::string::npos)
					{
						deps.put(std::string(""), dataSourceName);
						
					}
					else
					{
						if (localParent->getLevel() <= getLevel())
						{
							throw HipeException("Error : There is a dependencie upper than the " + getName());
						}
						deps.put(std::string(""), localParent->getName());
					}
						

					containerNeed.push_back("", deps);
				}

				currentFilter.add_child("need", containerNeed);
				json::JsonTree containerFilter;
				containerFilter.add_child(child->getConstructorName(), currentFilter);
				filterList.push_back("", containerFilter);
				//Go deeper in the tree
				if (!childMap.second->getChildrens().empty())
				{
					heap.push(child);
					level_max_node = std::max(child->getLevel(), level_max_node);
				}
			}
		}
	

	}
	if (level_max_node >= level_popGraph)
	{
		throw HipeException("Some part of graph over pass the popToNodeGraph. Please review the graph to export");
	}
	filterChild.put(std::string("orchestrator"), std::string("DefaultScheduler"));

	
	filterChild.add_child("filters", filterList);

	//Add SerialNetData ....
	json::JsonTree dataSource;
	dataSource.put("address", local_adress);
	dataSource.put("port", 3100);
	
	dataSource.put("name", dataSourceName);
	json::JsonTree filterDataSource;

	filterDataSource.add_child("SerialNetDataSource", dataSource);

	json::JsonTree dataSourceList;

	dataSourceList.push_back("", filterDataSource);

	json::JsonTree dataSourceKey;

	dataSourceKey.add_child("datasource", dataSourceList);

	filterChild.add_child("data", dataSourceKey);
	std::stringstream status;
	filterChild.write_json(status);
	std::cout << status.str() << std::endl;

	return status.str();
}

void filter::algos::PushGraphToNode::Connect(boost::asio::io_service & service)
{
	boost::asio::ip::tcp::socket s(service);
	boost::asio::ip::tcp::resolver resolver(service);

	std::stringstream build_port;
	build_port << target_port;
	/*boost::asio::async_connect(s, resolver.resolve({host_or_ip,  build_port.str() }), [](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator it) {
		if (ec) throw HipeException("Error connecting to server: " + ec.message());
		std::cout << "Connected to " << it->endpoint() << std::endl;
	});*/
	boost::system::error_code ec;

	boost::asio::connect(s, resolver.resolve({ host_or_ip,  build_port.str() }), ec);

	if (ec != 0)
	{
		throw HipeException("Alert slave unreachable");
	}
}

HipeStatus filter::algos::PushGraphToNode::sendGraphToService(const std::string& json_string, const std::string& address_port)
{
	std::stringstream build_adress;

	build_adress << this->host_or_ip << ":" << this->target_port;

	http::Client<http::HTTP> client(build_adress.str());
	std::string killComand = "{\"name\": 'Kill',\"command\" :	{\"type\": 'Kill'} };";
	auto r2 = client.request("POST", "/json", killComand);
	std::cout << r2->content.rdbuf() << std::endl;
	std::cout << "==== Kill previous task request ==== " << std::endl;

	auto r3 = client.request("POST", "/json", json_string);
	std::cout << r3->content.rdbuf() << std::endl;
	std::cout << "==== End of Json Post request ==== " << std::endl;

	

	return OK;
}

void filter::algos::PushGraphToNode::startSerialNetServer()
{
		//Cut port in host_or_ip if exist
		{
			std::string delimiter = ":";

			size_t pos = 0;
			std::vector<std::string> token;
			std::string target_adress = host_or_ip;

			while ((pos = target_adress.find(delimiter)) != std::string::npos) {
				token.push_back(target_adress.substr(0, pos));

				target_adress.erase(0, pos + delimiter.length());
			}
			//Push the last argument if pos != size
			if (target_adress != "")
			{
				token.push_back(target_adress);
			}

			if (token.size() > 2)
				throw HipeException("Invalid adress" + host_or_ip);
			if (token.size() == 2)
			{
				host_or_ip = token[0];
				std::string::size_type sz;   // alias of size_t

				target_port = std::stoi(token[1], &sz);

			}
			std::cout << "Slave Adress to request task : " << host_or_ip << ":" << target_port << std::endl;
		}


		{
			//Resolve local ip to use do make a p2p connection
			boost::asio::io_service io_service;
			boost::asio::ip::tcp::socket s(io_service);


			s.connect(boost::asio::ip::tcp::endpoint(
				boost::asio::ip::address::from_string(host_or_ip), target_port));

			local_adress = std::string(s.local_endpoint().address().to_string());
			std::cout << "Local adress are :" << local_adress << std::endl;
		}

		sender = SerialNetDataServer(3100, 1); // Warning need to verify the port to use when multiple slave are requested
		SerialNetDataServer::ptr_func func = &SerialNetDataServer::ImagesHandler;
		sender.startServer(3100, std::bind(func, &sender));


		//send graph to the node via UDP protocol to host_or_ip
		std::string json_request = rebuildJsonFromGraph();

		HipeStatus st = sendGraphToService(json_request, host_or_ip);
		if (st != OK)
		{
			a_isActive = false;
			HipeException("Cannot send graph to node " + host_or_ip);
		}

		
		//);


}


HipeStatus filter::algos::PushGraphToNode::process()
{
	if (!a_isActive.exchange(true))
	{
		startSerialNetServer();
	} // end of init

	while (_connexData.size() != 0)
	{
		data::ImageData value = _connexData.pop();
		//Initialisation Create Json graph from here to the next PopGraph node
	

		if (sender.isActive())
		//Serialize data and send it
			sender.imagesStack.push(value.getMat());
		else
		{
			return END_OF_STREAM;
		}
	}

	return OK;
}

/**
* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
*/
void  filter::algos::PushGraphToNode::dispose()
{
	a_isActive = false;
	sender.isActive() = false;
	sender.stop();
	
}