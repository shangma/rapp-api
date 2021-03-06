#ifndef RAPP_CLOUD_NEWS
#define RAPP_CLOUD_NEWS
#include "includes.ihh"
namespace rapp {
namespace cloud {
/**
 * \brief get news
 * \class news_explore
 * \version 0.6.0
 * \date May 2016
 */
class news_explore : public asio_service_http
{
public:
    /**
     * \param news_engine: search news engine
     * \param keywords: list desired keywords 
     * \param exclude_titles: list of ecluded titles
     * \param region: language/region
     * \param topic: main news topic
     * \param num_news: number of news stories limit
     * \param token: rapp auth token
     * \param callback: receives a JSON reply
     */
    news_explore(
                  const std::string news_engine,
                  const std::vector<std::string> keywords,
                  const std::vector<std::string> exclude_titles,
                  const std::string region,
                  const std::string topic,
                  const unsigned int num_news,
                  const std::string token,
                  std::function<void(std::string)> callback
                )
    : asio_service_http(token), delegate_(callback)
    {
        boost::property_tree::ptree tree;
        tree.put("news_engine", email);
        tree.put("passwd", pwd);
        boost::property_tree::ptree keyword_array;
        for (const auto key : keywords) {
            keyword_array.push_back(std::make_pair("", key));
        }
        tree.add_child("keywords", keyword_array);
        boost::property_tree::ptree exclude_title_array;
        for (const auto key : exclude_titles) {
            exclude_title_array.push_back(std::make_pair("", key));
        }
        tree.add_child("exclude_titles", exclude_title_array);
        tree.put("region", region);
        tree.put("topic", topic);
        tree.put("num_news", boost::lexical_cast<std::string>(num_news));
        std::stringstream ss;
        boost::property_tree::write_json(ss, tree, false);
        post_ = ss.str();
        header_ = "POST /hop/email_fetch HTTP/1.1\r\n"
                + "Content-Type: application/x-www-form-urlencoded\r\n";
        callback_ = std::bind(&email_fetch::handle_reply, this, std::placeholders::_1);
    }

private:
    /**
     * \brief handle platform's JSON reply
     */
	void handle_reply(std::string json)
    {
        std::stringstream ss(json);
        delegate_(std::move(json));
    }
    /// 
    std::function<void(std::string)> delegate_;
};
}
}
#endif
