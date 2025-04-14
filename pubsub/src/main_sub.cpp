
/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <spdlog/spdlog.h>
#include <unistd.h>
#include <up-cpp/communication/Subscriber.h>

#include <csignal>
#include <iostream>

#include "UTransportDomainSockets.h"
#include "common.h"

using namespace uprotocol::communication;
using namespace uprotocol::v1;

bool g_terminate = false;

void signalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "Ctrl+C received. Exiting..." << std::endl;
		g_terminate = true;
	}
}

void onReceiveTime(const UMessage& message) {
	if (message.attributes().payload_format() ==
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT) {
		const char* payload =
		    reinterpret_cast<const char*>(message.payload().data());
		spdlog::info("received time = {}", payload);
	}
}

void onReceiveRandom(const uprotocol::v1::UMessage& message) {
	if (message.attributes().payload_format() ==
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT) {
		const char* payload =
		    reinterpret_cast<const char*>(message.payload().data());
		spdlog::info("received random = {}", payload);
	}
}

void onReceiveCounter(const uprotocol::v1::UMessage& message) {
	if (message.attributes().payload_format() ==
	    UPayloadFormat::UPAYLOAD_FORMAT_TEXT) {
		const char* payload =
		    reinterpret_cast<const char*>(message.payload().data());
		spdlog::info("received counter = {}", payload);
	}
}

/* The sample sub applications demonstrates how to consume data using uTransport
 * -
 * There are three topics that are received - random number, current time and a
 * counter */
int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	(void)signal(SIGINT, signalHandler);
	(void)signal(SIGPIPE, signalHandler);

	UStatus status;
	UUri source = getUUri(0);
	const auto& topic_time = getTimeUUri();
	const auto& topic_random = getRandomUUri();
	const auto& topic_counter = getCounterUUri();
	auto transport = std::make_shared<UTransportDomainSockets>(source);

	auto res_time =
	    Subscriber::subscribe(transport, topic_time, onReceiveTime);
	auto res_random = Subscriber::subscribe(transport, topic_random,
	                                       onReceiveRandom);
	auto res_counter = Subscriber::subscribe(transport, topic_counter,
	                                        onReceiveCounter);

	while (!g_terminate) {
		sleep(1);
	}

	return 0;
}
