#ifndef RAPP_CLOUD_VISUAL_LOCALIZATION
#define RAPP_CLOUD_VISUAL_LOCALIZATION
#include "includes.ihh"
namespace rapp {
namespace cloud {

//#define PRINT_JSON

namespace pt = boost::property_tree;



/**
 * \class visual_localization_init
 * \brief load map
 * \version 0.6.1
 * \date November 2016
 * \author Maciej Stefańczyk <M.Stefanczyk@elka.pw.edu.pl>
 */
class visual_localization_init : public asio_http
{
public:
    /**
    * \brief Constructor
    * \param callback is the function
    */
    visual_localization_init(
                      const std::string & map,
                      std::function<void(int)> callback
                    )
    : asio_http(), delegate__(callback)
    {
        std::string boundary = rapp::misc::random_boundary();

        boost::property_tree::ptree tree;
        tree.put("map", map, s());

        std::stringstream ss;
        boost::property_tree::write_json(ss, tree, false);
        
#ifdef PRINT_JSON
        std::cout << "/-------------------------\n";
        std::cout << ss.str() << std::endl;
        std::cout << "\\-------------------------\n";
#endif

        post_  = "--" + boundary + "\r\n"
               + "Content-Disposition: form-data; name=\"json\"\r\n\r\n"
               + ss.str();
        
        post_ += "--"+boundary+"--";

        // set the HTTP header URI pramble and the Content-Type
        head_preamble_.uri = "POST /hop/visual_localization_init HTTP/1.1\r\n";
        head_preamble_.content_type = "Content-Type: multipart/form-data; boundary=" + boundary;

        callback_ = std::bind(&visual_localization_init::handle_reply, this, std::placeholders::_1);   
    }
private:
    /**
     * \brief handle the rapp-platform JSON reply
     */
    void handle_reply(std::string json)
    {
        std::stringstream ss(json);
        int result = 0;
        try {
            boost::property_tree::ptree tree;
            boost::property_tree::read_json(ss, tree);

#ifdef PRINT_JSON
            std::cout << "/-RESP--------------------\n";
            std::cout << json << std::endl;
            std::cout << "\\-------------------------\n";
#endif

            result = tree.get<int>("id", -1);

            // Check for Errors returned by the platform
            for (auto child : tree.get_child("error")) {
                const std::string value = child.second.get_value<std::string>();
                if (!value.empty()) {
                    std::cerr << "visual_localization JSON error: " << value << std::endl;
                }
            }
        }
        catch(boost::property_tree::json_parser::json_parser_error & je) {
            std::cerr << "visual_localization::handle_reply Error parsing: " 
                      << je.filename() << " on line: " << je.line() << std::endl;
            std::cerr << je.message() << std::endl;
        }
        delegate__(result);
    }

    /// The callback called upon completion of receiving the detected faces
    std::function<void(int)> delegate__;
};


/**
 * \class visual_localization
 * \brief global localization using visual information
 * \version 0.6.1
 * \date November 2016
 * \author Maciej Stefańczyk <M.Stefanczyk@elka.pw.edu.pl>
 */
class visual_localization : public asio_http
{
public:
    /**
    * \brief Constructor
    * \param id is an identifier for loaded map
    * \param image is a picture object pointer
    * \param pose_delta is a change in position/orientation from last measurement
    * \param callback is the function that will receive an estimated position/orientation
    */
    visual_localization(
                      int id,
                      const rapp::object::picture::Ptr & image,
                      const rapp::object::point & pose_delta,
                      std::function<void(rapp::object::point, float, int)> callback
                    )
    : asio_http(), delegate__(callback)
    {
        std::string boundary = rapp::misc::random_boundary();
        std::string fname = rapp::misc::random_boundary() + "." + image->type();

        boost::property_tree::ptree tree;
        //tree.put("file", fname, s());
        tree.put("id", id);
        tree.put("pose_delta.x", pose_delta.x);
        tree.put("pose_delta.y", pose_delta.y);
        tree.put("pose_delta.theta", pose_delta.z);
        std::stringstream ss;
        boost::property_tree::write_json(ss, tree, false);

#ifdef PRINT_JSON
        std::cout << "/-REQ---------------------\n";
        std::cout << ss.str() << std::endl;
        std::cout << "\\-------------------------\n";
#endif
        
        post_  = "--" + boundary + "\r\n"
               + "Content-Disposition: form-data; name=\"json\"\r\n\r\n"
               + ss.str() + "\r\n";

		post_ += "--" + boundary + "\r\n"
              + "Content-Disposition: form-data; name=\"file\"; filename=\"" + fname + "\"\r\n"
              + "Content-Type: image/" + image->type() + "\r\n"
              + "Content-Transfer-Encoding: binary\r\n\r\n";

        // Append binary data
        auto imagebytes = image->bytearray();
        post_.insert(post_.end(), imagebytes.begin(), imagebytes.end());
        post_ += "\r\n";
        post_ += "--"+boundary+"--";

        // set the HTTP header URI pramble and the Content-Type
        head_preamble_.uri = "POST /hop/visual_localization HTTP/1.1\r\n";
        head_preamble_.content_type = "Content-Type: multipart/form-data; boundary=" + boundary;

        callback_ = std::bind(&visual_localization::handle_reply, this, std::placeholders::_1);   
    }
private:
    /**
     * \brief handle the rapp-platform JSON reply
     */
    void handle_reply(std::string json)
    {
        std::stringstream ss(json);
        rapp::object::point pose;
        float belief;
        int status;
        try {
            boost::property_tree::ptree tree;
            boost::property_tree::read_json(ss, tree);

#ifdef PRINT_JSON
            std::cout << "/-RESP--------------------\n";
            std::cout << json << std::endl;
            std::cout << "\\-------------------------\n";
#endif

            pose.x = tree.get<float>("best_pose.x");
            pose.y = tree.get<float>("best_pose.y");
            pose.z = tree.get<float>("best_pose.theta");

            status = tree.get<int>("status", 0);
            belief = tree.get<float>("belief", 0);

            // Check for Errors returned by the platform
            std::string value = tree.get<std::string>("error", "");
            if (!value.empty()) {
                std::cerr << "object_detection JSON error: " << value << std::endl;
            }
        }
        catch(boost::property_tree::json_parser::json_parser_error & je) {
            std::cerr << "object_detection::handle_reply Error parsing: " 
                      << je.filename() << " on line: " << je.line() << std::endl;
            std::cerr << je.message() << std::endl;
        }
        delegate__(pose, belief, status);
    }

    /// The callback called upon completion of receiving the detected faces
    std::function<void(rapp::object::point, float, int)> delegate__;
};
}
}
#endif