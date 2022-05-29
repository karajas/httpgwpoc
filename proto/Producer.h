#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "httpgateway.pb.h"
#include "httpgateway.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using httpgateway::ProduceRequest;
using httpgateway::ProduceResponse;
using httpgateway::HttpGateway;

struct ProducerConfigSetting
{
	std::string key;
	std::string value;
};

class HttpGatewayClient
{
public:
    HttpGatewayClient(std::string target_str)
	{
        m_channel = grpc::CreateChannel(
			target_str, 
            grpc::InsecureChannelCredentials());
	};

    std::unique_ptr<Producer> CreateProducer()
    {
        return std::make_unique<Producer>(m_channel);
    }

private:
    std::shared_ptr<Channel> m_channel;
};

class Producer
{
public:
	Producer(std::shared_ptr<grpc::ChannelInterface> channel)
    {
        stub_ = HttpGateway::NewStub(channel);
    };

    std::string Produce(const std::string& user)
    {
        ProduceRequest request;
        std::string mytopic("tangent");
        request.set_topic(mytopic);

        ProduceResponse reply;

        ClientContext context;
        Status status = stub_->Produce(&context, request, &reply);

        if (status.ok())
        {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return "RPC response: " + std::to_string(reply.partition()) + " " + std::to_string(reply.offset());
        }
        else
        {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<HttpGateway::Stub> stub_;
	std::vector<ProducerConfigSetting> producerConfig;
};