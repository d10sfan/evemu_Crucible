/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2008 The EVEmu Team
	For the latest information visit http://evemu.mmoforge.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Zhur
*/

#include "EVECommonPCH.h"

#include "marshal/EVEMarshal.h"
#include "marshal/EVEUnmarshal.h"
#include "network/EVETCPConnection.h"

const uint32 EVETCPCONN_TIMEOUT = 10 * 60 * 1000; // 10 minutes
const uint32 EVETCPCONN_PACKET_LIMIT = 10485760;

//client side
EVETCPConnection::EVETCPConnection()
: TCPConnection(),
  mTimeoutTimer( EVETCPCONN_TIMEOUT )
{
}

//server side case
EVETCPConnection::EVETCPConnection( Socket* sock, uint32 rIP, uint16 rPort )
: TCPConnection( sock, rIP, rPort ),
  mTimeoutTimer( EVETCPCONN_TIMEOUT )
{
}

void EVETCPConnection::QueueRep( const PyRep* rep )
{
    uint32 length;
    uint8* buf = MarshalDeflate( rep, length );

    if( buf == NULL )
        sLog.Error( "Network", "Failed to marshal new packet." );
    else if( length > EVETCPCONN_PACKET_LIMIT )
        sLog.Error( "Network", "Packet length %u exceeds hardcoded packet length limit %u.", length, EVETCPCONN_PACKET_LIMIT );
    else
        ServerSendQueuePushEnd( (const uint8 *)&length, sizeof( length ), &buf, length );

	SafeDelete( buf );
}

PyRep* EVETCPConnection::PopRep()
{
    mMInQueue.lock();
    StreamPacketizer::Packet* p = mInQueue.PopPacket();
	mMInQueue.unlock();

    PyRep* res = NULL;
    if( p != NULL )
    {
        if( p->length > EVETCPCONN_PACKET_LIMIT )
            sLog.Error( "Network", "Packet length %u exceeds hardcoded packet length limit %u.", p->length, EVETCPCONN_PACKET_LIMIT );
        else
            res = InflateUnmarshal( p->data, p->length );
    }

    SafeDelete( p );
	return res;
}

bool EVETCPConnection::ProcessReceivedData( char* errbuf )
{
	if( errbuf )
		errbuf[0] = 0;

	if( mRecvBuf == NULL || mRecvBufSize == 0 || mRecvBufUsed == 0 )
		return true;

	mTimeoutTimer.Start();

    LockMutex lock( &mMInQueue );
    // put bytes into packetizer
    mInQueue.InputBytes( mRecvBuf, mRecvBufUsed );

    // delete recieve queue
    SafeDelete( mRecvBuf );
    mRecvBufSize = mRecvBufUsed = 0;

    return true;
}

bool EVETCPConnection::RecvData( char* errbuf )
{
	if( !TCPConnection::RecvData( errbuf ) )
        return false;
	
	if( mTimeoutTimer.Check() )
    {
		if( errbuf )
			snprintf( errbuf, TCPCONN_ERRBUF_SIZE, "EVETCPConnection::RecvData(): Connection timeout" );
		return false;
	}

	return true;
}

void EVETCPConnection::ClearBuffers()
{
	TCPConnection::ClearBuffers();

    mTimeoutTimer.Start();

    LockMutex lock( &mMInQueue );
    mInQueue.ClearBuffers();
}
