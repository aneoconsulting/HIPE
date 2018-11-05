#include <iostream>
#include <http/HttpServer.h>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <algorithm>
#include <boost/asio/ip/tcp.hpp>

#include <openssl/base.h>

#include "tools/net/WebsocketServer.h"
#include <glog/logging.h>
#include <boost/asio/ssl/context.hpp>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;



std::string get_password() {
    return "test";
}

// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
// the TLS modes. The code below demonstrates how to implement both the modern
enum tmode {
    MOZILLA_INTERMEDIATE = 1,
    MOZILLA_MODERN = 2
};

context_ptr on_tinit(tmode mode, std::string basename, websocketpp::connection_hdl hdl) {

    namespace asio = websocketpp::lib::asio;
	
    LOG(INFO) << "on_tinit called with hdl: " << hdl.lock().get();
    LOG(INFO) << "using TLS mode: " << (mode == MOZILLA_MODERN ? "Mozilla Modern" : "Mozilla Intermediate");
	LOG(INFO) << "Pem file : " << basename << ".crt";
	LOG(INFO) << "create asio context shared ptr";
	
    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

    try {
        if (mode == MOZILLA_MODERN) {
			LOG(INFO) << "Set options MODERN";
            // Modern disables TLSv1
            ctx->set_options(asio::ssl::context::default_workarounds |
	            asio::ssl::context::no_sslv2 |
	            asio::ssl::context::no_sslv3 |
	            asio::ssl::context::no_tlsv1 |
	            asio::ssl::context::single_dh_use);
        } else {
			LOG(INFO) << "Set options INTERMEDIATE";

            ctx->set_options(asio::ssl::context::default_workarounds |
	            asio::ssl::context::no_sslv2 |
	            asio::ssl::context::no_sslv3 |
	            asio::ssl::context::single_dh_use);
        }
		LOG(INFO) << "Set callback fort password";

        ctx->set_password_callback(bind(&get_password));

		LOG(INFO) << "Set perm crt : " << basename;
        ctx->use_certificate_chain_file(basename + ".crt");

		LOG(INFO) << "Set perm key";
        ctx->use_private_key_file(basename + ".key", asio::ssl::context::pem);
        
        // Example method of generating this file:
        // `openssl dhparam -out dh.pem 2048`
        // Mozilla Intermediate suggests 1024 as the minimum size to use
        // Mozilla Modern suggests 2048 as the minimum size to use.
//        ctx->use_tmp_dh_file("dh.pem");
        
        std::string ciphers;
        
        if (mode == MOZILLA_MODERN) {
            ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
        } else {
            ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
        }

		LOG(INFO) << "Set SSL_CTX_set_cipher";

        if (SSL_CTX_set_cipher_list(ctx->native_handle() , ciphers.c_str()) != 1) {
            LOG(INFO) << "Error setting cipher list";
        }
    } catch (std::exception& e) {
        LOG(INFO) << "Exception: " << e.what();
    }

    LOG(INFO) << "ssl init end.";
    return ctx;
}

WebsocketServer* ServerInit(std::function<void(WebsocketServer*, websocketpp::connection_hdl)> con_callback,
	std::function<void(WebsocketServer*, websocketpp::connection_hdl)> open_callback,
	std::function<void(WebsocketServer*, websocketpp::connection_hdl)> ccallback,
	std::function<void(WebsocketServer*, websocketpp::connection_hdl, message_ptr)> msg_callback,
	std::string basename
) {

  // Create a server endpoint
  auto websocket_server = new WebsocketServer(basename);

  LOG(INFO) << "Certificats path " << basename;
  // Register our message handler
  websocket_server->onMessageHandler(msg_callback);
  websocket_server->onConnectionHandler(con_callback);
  websocket_server->onOpenHandler(open_callback);
  websocket_server->onCloseHandler(ccallback);
  websocket_server->set_tls_init_handler(bind(&on_tinit,MOZILLA_INTERMEDIATE, basename, ::_1));
  

  
  // Start the ASIO io_service run loop
  // DO NOT websocket_server.poll(); here!! becasu it is block.

  return websocket_server;
}
