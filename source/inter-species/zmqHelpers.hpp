/*!
    Convenience functions and classes for zmq.

    Comes from https://github.com/larics/assisi-playground/tree/master/playground
 */

#ifndef CATS2_ZMQ_HELPERS_HPP
#define CATS2_ZMQ_HELPERS_HPP

#include <zmq.hpp>

//#include <iostream>

namespace zmq
{

// -----------------------------------------------------------------------------

    //! Check if this is the last part of a multipart message.
    /*! Returns true if there are no further parts.
     */
    inline bool lastPart(socket_t& socket)
    {
        int64_t more;
        size_t more_size = sizeof(more);
        socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        return  (more == 1) ? false : true;
    }
// -----------------------------------------------------------------------------

    //! Convert message data to string.
    inline std::string msgToStr(message_t& msg)
    {
        return std::string(static_cast<char*>(msg.data()), 
                           msg.size());
    }

// -----------------------------------------------------------------------------

    //! Convert string to message data.
    inline void strToMsg(const std::string& str, message_t& msg)
    {
        msg.rebuild(str.size());
        memcpy(msg.data(), str.data(), str.size());
    }

// -----------------------------------------------------------------------------

    //! Receive all parts of a multipart message.
    /*!
        Functions are specific to the assisi message protocol,
        i.e., each message is a multipart message in the format:
        
        +---------+
        | header1 |
        +---------+
        | header2 |
        +---------+
        | header3 |
        +---------+
        |  data   |
        +---------+

        The function asserts the above message format.
    */
    static int recvMultipart(socket_t& socket,
                       std::string& name,
                       std::string& device,
                       std::string& command,
                       std::string& data,
                       int flags = 0)
    {
        int len = 0;
        message_t msg;
        
        len = socket.recv(&msg, flags);
        if (len > 0)
        {
            name = msgToStr(msg);
            len += socket.recv(&msg);
            device = msgToStr(msg);

            len += socket.recv(&msg);
            command = msgToStr(msg);
            
            len += socket.recv(&msg);
            data = msgToStr(msg);
        }
        return len;
    }
                                  
// -----------------------------------------------------------------------------

    //! Send a multipart message.
    /*! Outgoing messages conform to the sam format as incoming mssages,
        i.e., they have 4 header parts and a data part.
    */
    static int sendMultipart(socket_t& socket,
                       const std::string& name,
                       const std::string& device,
                       const std::string& desc,
                       const std::string& data,
                       int flags = 0)
    {
        message_t msg;
        strToMsg(name, msg);
        socket.send(msg, ZMQ_SNDMORE);
        strToMsg(device, msg);
        socket.send(msg, ZMQ_SNDMORE);
        strToMsg(desc, msg);
        socket.send(msg, ZMQ_SNDMORE);
        strToMsg(data, msg);
        socket.send(msg);
               
        return 4;
    }
}

#endif // CATS2_ZMQ_HELPERS_HPP
