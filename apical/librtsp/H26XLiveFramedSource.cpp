/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2018 Live Networks, Inc.  All rights reserved.
// Framed File Sources
// Implementation

#include "OnDemandRTSPServer.h"
#include "H26XLiveFramedSource.hh"

////////// FramedFileSource //////////

H26XLiveFramedSource*
H26XLiveFramedSource::createNew(UsageEnvironment& env, CONTEXT* ctxt) {
    H26XLiveFramedSource* newSource = new H26XLiveFramedSource(env, ctxt);
    return newSource;
}

H26XLiveFramedSource::H26XLiveFramedSource(UsageEnvironment& env, CONTEXT* ctxt)
    : FramedSource(env), mContext(ctxt), mMaxFrameSize(128*1024) {
}

H26XLiveFramedSource::~H26XLiveFramedSource() {
}

void H26XLiveFramedSource::doGetNextFrame() {
    int nread = rtsp_get_vframe(mContext, fTo, fMaxSize, (int*)&fFrameSize);
    fNumTruncatedBytes = fFrameSize - nread;
    fDurationInMicroseconds = nread ? 1000000 / 15 : 0;
    gettimeofday(&fPresentationTime, NULL);
    if (fFrameSize > 0 && mMaxFrameSize < fFrameSize) mMaxFrameSize = fFrameSize;

    // To avoid possible infinite recursion, we need to return to the event loop to do this:
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
    if (mContext->bexit) handleClosure();
}

