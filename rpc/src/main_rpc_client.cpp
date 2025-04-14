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
#include <up-cpp/communication/RpcClient.h>

#include <chrono>
#include <csignal>
#include <iostream>

#include "SocketUTransport.h"
#include "common.h"

using namespace uprotocol::v1;

bool g_terminate = false;

void signalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "Ctrl+C received. Exiting..." << std::endl;
		g_terminate = true;
	}
}

void OnReceive(uprotocol::communication::RpcClient::MessageOrStatus expected) {
	if (!expected.has_value()) {
		const UStatus& status = expected.error();
		spdlog::error("Expected value not found. -- Status: {}",
		              status.DebugString());
		return;
	}

	UMessage message = std::move(expected).value();

	if (message.attributes().payload_format() !=
	    UPayloadFormat::UPAYLOAD_FORMAT_RAW) {
		spdlog::error("Received message has unexpected payload format:\n{}",
		              message.DebugString());
		return;
	}

	if (message.payload().size() != (sizeof(uint64_t) * 3)) {
		spdlog::error("Received message has unexpected payload size:\n{}",
		              message.DebugString());
		return;
	}

	// RPC response is expected to have a payload of 3 uint64_t values
	// sequence number, current time, and random value
	spdlog::debug("(Client) Received message: {}", message.DebugString());

	std::vector<uint64_t> pdata(3);
	size_t expected_size = 3 * sizeof(uint64_t);
	std::memcpy(pdata.data(), message.payload().data(), expected_size);
	spdlog::info("Received payload: {} - {}, {}", pdata[0], pdata[1], pdata[2]);
}

/* The sample RPC client applications demonstrates how to send RPC requests and
 * wait for the response
 */
int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	(void)signal(SIGINT, signalHandler);

	UUri source = getRpcUUri(0);
	UUri method = getRpcUUri(12);
	auto transport = std::make_shared<SocketUTransport>(source);
	auto client =
	    uprotocol::communication::RpcClient(transport, std::move(method), UPriority::UPRIORITY_CS4,
	              std::chrono::milliseconds(500));
	uprotocol::communication::RpcClient::InvokeHandle handle;

	while (!g_terminate) {
		handle = client.invokeMethod(OnReceive);
		sleep(1);
	}

	return 0;
}

