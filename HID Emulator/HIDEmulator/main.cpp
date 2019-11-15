// 
// main.cpp
// Created by ilian on 2015-02-07
// Licenced under the Apache License, Version 2.0
//

#define OSCPKT_OSTREAM_OUTPUT
extern "C" {
#include <interception.h>
}
#include "utils.h"
#include <iostream>
#include "oscpkt.hh"
#include "udp.hh"
#include <chrono>
#include <thread>

using std::cout;
using std::cerr;
using std::string;

using namespace oscpkt;

const int PORT_NUM = 57120;

enum ScanCode
{
	// Player 1
	SCANCODE_Z = 0x2c,
	SCANCODE_S = 0x1f,
	SCANCODE_X = 0x2D,
	SCANCODE_D = 0x20,
	SCANCODE_C = 0x2e,
	SCANCODE_F = 0x21,
	SCANCODE_V = 0x2f,
	SCANCODE_BRACKET_OPEN = 0x1a,
	SCANCODE_QUOTATIONMARK = 0x28,
	// Player 2
	SCANCODE_M = 0x32,
	SCANCODE_K = 0x25,
	SCANCODE_COMMA = 0x33,
	SCANCODE_L = 0x26,
	SCANCODE_DOT = 0x34,
	SCANCODE_SEMICOLON = 0x27,
	SCANCODE_FORWARDSLASH = 0x35,
	SCANCODE_BRACKET_CLOSE=0x1b,
	SCANCODE_HASH = 0x2b,
	// CHUNITHM
	SCANCODE_1 = 0x02,
	SCANCODE_2 = 0x03,
	SCANCODE_3 = 0x04,
	SCANCODE_4 = 0x05,
	SCANCODE_5 = 0x06,
	SCANCODE_6 = 0x07,
	SCANCODE_7 = 0x08,
	SCANCODE_8 = 0x09,
	SCANCODE_Q = 0x10,
	SCANCODE_W = 0x11,
	SCANCODE_E = 0x12,
	SCANCODE_R = 0x13,
	SCANCODE_T = 0x14,
	SCANCODE_Y = 0x15,
	SCANCODE_U = 0x16,
	SCANCODE_I = 0x17,
	SCANCODE_A = 0x1e,
	SCANCODE_G = 0x22,
	SCANCODE_H = 0x23,
	SCANCODE_J = 0x24,
	SCANCODE_B = 0x30,
	SCANCODE_N = 0x31,
	SCANCODE_CONMA = 0x33

};

// Thread that passes through keyboard input
void interceptThread(InterceptionContext* context) {
	InterceptionDevice device;
	InterceptionKeyStroke stroke;
	while (interception_receive(*context, device = interception_wait(*context), (InterceptionStroke *)&stroke, 1) > 0) {
		interception_send(*context, device, (InterceptionStroke *)&stroke, 1);
		cout << std::showbase << "Keyboard button " << std::hex << stroke.code << " pressed." << std::endl;
	}
}

int main()
{
	UdpSocket sock;
	sock.bindTo(PORT_NUM);

	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke;
	raise_process_priority();
	context = interception_create_context();

	cout << "Please press any key on your keyboard to be selected for emulation." << std::endl;
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);
	device = interception_wait(context);
	std::thread first(interceptThread, (InterceptionContext *)&context );
	if (!sock.isOk()) {
		cerr  << "Error opening port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
	}
	else {
		cout << std::dec << "Server started, will listen to packets on port " << PORT_NUM << std::endl;
		PacketReader pr;
		PacketWriter pw;
		while (sock.isOk()) {
			if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
				pr.init(sock.packetData(), sock.packetSize());
				oscpkt::Message *msg;
				while (pr.isOk() && (msg = pr.popMessage()) != 0) {
					int state;
					string player;
					cout << "Server: received message " << msg->addressPattern() <<" from " << sock.packetOrigin() << "\n";
					if (msg->match("/iidx_1").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_Z : SCANCODE_M;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_2").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_S : SCANCODE_K;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_3").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_X : SCANCODE_COMMA;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_4").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_D : SCANCODE_L;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_5").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_C : SCANCODE_DOT;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_6").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_F : SCANCODE_SEMICOLON;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/iidx_7").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_V : SCANCODE_FORWARDSLASH;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/turntable_cw").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1"? SCANCODE_BRACKET_OPEN: SCANCODE_BRACKET_CLOSE;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/turntable_ccw").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = player == "1" ? SCANCODE_QUOTATIONMARK: SCANCODE_HASH;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_1").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code =  SCANCODE_1 ;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_2").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_Q;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_3").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_2;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_4").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_W;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_5").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_3;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_6").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_E;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_7").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_4;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_8").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_R;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_9").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_5;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_10").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_T;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_11").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_6;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_12").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_Y;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_13").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_7;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_14").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_U;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_15").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_8;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_16").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_I;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_17").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_A;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_18").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_Z;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_19").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_S;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_20").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_X;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_21").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_D;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_22").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_C;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_23").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_F;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_24").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_V;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_25").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_G;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_26").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_B;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_27").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_H;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_28").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_N;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_29").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_J;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_30").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_M;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_31").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_K;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}
					else if (msg->match("/jubeat_31").popInt32(state).popStr(player).isOkNoMoreArgs()) {
						stroke.code = SCANCODE_CONMA;
						stroke.state = state == 1 ? 0x00 : 0x01;
					}

					else {
						cout << "Server: unhandled message: " << *msg << "\n";
					}
					interception_send(context, device, (InterceptionStroke *)&stroke, 1);
				}
			}
		}
	}
	interception_destroy_context(context);
	return 0;
}
