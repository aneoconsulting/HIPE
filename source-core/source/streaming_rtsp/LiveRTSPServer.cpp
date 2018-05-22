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

//
//  LiveRTSPServer.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "LiveRTSPServer.h"

namespace MESAI
{
	LiveRTSPServer::LiveRTSPServer( FFmpegH264Encoder * a_Encoder, int port, int httpPort )
		: m_Encoder (a_Encoder), portNumber(port), httpTunnelingPort(httpPort)
	{
		quit = 0;
	}

	LiveRTSPServer::~LiveRTSPServer()
	{

	}

	void LiveRTSPServer::run()
	{
		TaskScheduler    *scheduler;
		UsageEnvironment *env ;
		char RTSP_Address[1024];
		RTSP_Address[0]=0x00;

        scheduler = BasicTaskScheduler::createNew();
        env = BasicUsageEnvironment::createNew(*scheduler);
        
        UserAuthenticationDatabase* authDB = NULL;
        
        // if (m_Enable_Pass){
        // 	authDB = new UserAuthenticationDatabase;
        // 	authDB->addUserRecord(UserN, PassW);
        // }
        
        OutPacketBuffer::maxSize = 2000000;
        RTSPServer* rtspServer = RTSPServer::createNew(*env, portNumber, authDB);
        
        if (rtspServer == NULL)
        {
            *env <<"LIVE555: Failed to create RTSP server: %s\n", env->getResultMsg();
        }
        else {
            
            
            if(httpTunnelingPort)
            {
                rtspServer->setUpTunnelingOverHTTP(httpTunnelingPort);
            }
            
            char const* descriptionString = "MESAI Streaming Session";
            
            FFmpegH264Source * source = FFmpegH264Source::createNew(*env,m_Encoder);
            StreamReplicator * inputDevice = StreamReplicator::createNew(*env, source, false);
            
            ServerMediaSession* sms = ServerMediaSession::createNew(*env, RTSP_Address, RTSP_Address, descriptionString);
            sms->addSubsession(MESAI::LiveServerMediaSubsession::createNew(*env, inputDevice));
            rtspServer->addServerMediaSession(sms);
            
            char* url = rtspServer->rtspURL(sms);
            *env << "Play this stream using the URL \"" << url << "\"\n";
            delete [] url;
            
            //signal(SIGNIT,sighandler);
            env->taskScheduler().doEventLoop(&quit); // does not return
            
            Medium::close(rtspServer);
            Medium::close(inputDevice);
        }
        
        env->reclaim();
        delete scheduler;
	}
}
