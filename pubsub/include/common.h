// Copyright (c) 2024 General Motors GTO LLC
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 General Motors GTO LLC
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_H
#define COMMON_H

#include <uprotocol/v1/uri.pb.h>

inline uprotocol::v1::UUri getUUri(int const resource_id) {
	constexpr uint32_t PUBSUB_UE_ID = 0x18002;
	uprotocol::v1::UUri uuri;
	uuri.set_authority_name("test.app");
	uuri.set_ue_id(PUBSUB_UE_ID);
	uuri.set_ue_version_major(1);
	uuri.set_resource_id(resource_id);
	return uuri;
}

inline uprotocol::v1::UUri const& getTimeUUri() {
	constexpr uint32_t TIME_UURI_RESOURCE_ID = 0x8001;
	static auto uuri = getUUri(TIME_UURI_RESOURCE_ID);
	return uuri;
}

inline uprotocol::v1::UUri const& getRandomUUri() {
	constexpr uint32_t RANDOM_UURI_RESOURCE_ID = 0x8002;
	static auto uuri = getUUri(RANDOM_UURI_RESOURCE_ID);
	return uuri;
}

inline uprotocol::v1::UUri const& getCounterUUri() {
	constexpr uint32_t COUNTER_UURI_RESOURCE_ID = 0x8003;
	static auto uuri = getUUri(COUNTER_UURI_RESOURCE_ID);
	return uuri;
}

#endif  // COMMON_H
