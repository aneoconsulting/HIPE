//
//  main.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "LiveRTSPServer.h"
#include "FFmpegH264Encoder.h"
#include "FFmpegDecoder.h"
#include <cstdint>
#include <thread>

MESAI::FFmpegH264Encoder * encoder;
MESAI::LiveRTSPServer * server;
MESAI::FFmpegDecoder * decoder;

int UDPPort;
int HTTPTunnelPort;
std::thread thread1;
std::thread thread2;


void * runServer(void * server)
{
    ((MESAI::LiveRTSPServer * ) server)->run();
	return nullptr;
}

void * runEncoder(void * encoder)
{
    ((MESAI::FFmpegH264Encoder * ) encoder)->run();

	return nullptr;
}

void onFrame(uint8_t * data)
{
    encoder->SendNewFrame(data);
}

int main(int argc, const char * argv[])
{
    if(argc==2)
        decoder = new MESAI::FFmpegDecoder(argv[1]);
    if(argc==3)
        UDPPort = atoi(argv[2]);
    if(argc==4)
        HTTPTunnelPort = atoi(argv[3]);
    decoder->intialize();
    decoder->setOnframeCallbackFunction(onFrame);
    encoder = new MESAI::FFmpegH264Encoder();
    encoder->SetupVideo("dummy.avi",decoder->width,decoder->height,decoder->frameRate,decoder->GOP,decoder->bitrate);
    server = new MESAI::LiveRTSPServer(encoder, UDPPort, HTTPTunnelPort);

	thread1 = std::thread([] { runServer(server); });
	thread2 = std::thread([] { runEncoder(encoder); });

	thread1.detach();
	thread2.detach();

    // Play Media Here
    decoder->playMedia();
    decoder->finalize();
    
}
