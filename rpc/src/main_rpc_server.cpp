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
#include <up-cpp/communication/RpcServer.h>

#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <random>

#include "SocketUTransport.h"
#include "common.h"

constexpr uint32_t METHOD_RPC_RESOURCE_ID = 12;

namespace uprotocol::v1 {

bool g_terminate = false;

void signalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "Ctrl+C received. Exiting..." << std::endl;
		g_terminate = true;
	}
}

std::optional<uprotocol::datamodel::builder::Payload> OnReceive(
    const UMessage& message) {
	// Validate message is an RPC request
	if (message.attributes().type() != UMessageType::UMESSAGE_TYPE_REQUEST) {
		spdlog::error("Received message is not a request\n{}",
		              message.DebugString());
		return {};
	}

	// Validate message has no payload (no payload expected)
	if (message.has_payload()) {
		spdlog::error("Received message has non-empty payload\n{}",
		              message.DebugString());
		return {};
	}

	// Received request with empty payload, generate response with
	// sequence number, current time, and random value
	static uint64_t seq_num = 0;
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
	uint64_t rand_val = distribution(gen);
	uint64_t time_val = std::chrono::duration_cast<std::chrono::milliseconds>(
	                        std::chrono::system_clock::now().time_since_epoch())
	                        .count();
	std::vector<uint64_t> payload_data = {seq_num++, time_val, rand_val};

	spdlog::debug("(Server) Received request:\n{}", message.DebugString());

	uprotocol::datamodel::builder::Payload payload(
	    reinterpret_cast<std::vector<uint8_t>&>(payload_data),
	    UPayloadFormat::UPAYLOAD_FORMAT_RAW);
	spdlog::info("Sending payload:  {} - {}, {}", payload_data[0],
	             payload_data[1], payload_data[2]);

	return payload;
}
}  // namespace uprotocol::v1

/* The sample RPC server applications demonstrates how to receive RPC requests
 * and send a response back to the client -
 * The response in this example will be the current time */
int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	(void)signal(SIGINT, uprotocol::v1::signalHandler);

	uprotocol::v1::UUri source = getRpcUUri(0);
	uprotocol::v1::UUri method = getRpcUUri(METHOD_RPC_RESOURCE_ID);
	auto transport = std::make_shared<SocketUTransport>(source);
	auto server = uprotocol::communication::RpcServer::create(
	    transport, method, uprotocol::v1::OnReceive);

	if (!server.has_value()) {
		spdlog::error("Failed to create RPC server: {}",
		              server.error().DebugString());
		return 1;
	}

	while (!uprotocol::v1::g_terminate) {
		sleep(1);
	}

	return 0;
}
