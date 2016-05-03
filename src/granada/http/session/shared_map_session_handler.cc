/**
  * Copyright (c) <2016> granada <afernandez@cookinapps.io>
  *
  * This source code is licensed under the MIT license.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  * Manage the lifecycle of the sessions, store them in a map shared by all users.
  *
  */

#include "granada/http/session/shared_map_session_handler.h"

namespace granada{
  namespace http{
    namespace session{

      SharedMapSessionHandler::SharedMapSessionHandler() : SessionHandler(){
        LoadProperties();

        n_generator_ = std::unique_ptr<utility::nonce_generator>(new utility::nonce_generator(token_length_));
        sessions_ = std::unique_ptr<std::map<std::string,std::shared_ptr<granada::http::session::Session>>>(new std::map<std::string,std::shared_ptr<granada::http::session::Session>>);

        // thread for cleaning the sessions.
        if (clean_sessions_frequency_>-1){
          pplx::create_task([this]{
            this->CleanSessions(true);
          });
        }
      }

      void SharedMapSessionHandler::LoadProperties(){
        std::string clean_sessions_frequency_str(GetProperty("session_clean_frequency"));
        if (clean_sessions_frequency_str.empty()){
          clean_sessions_frequency_ = DEFAULT_CLEAN_SESSIONS_FREQUENCY;
        }else{
          try{
            clean_sessions_frequency_ = std::stod(clean_sessions_frequency_str);
          }catch(const std::exception& e){
            clean_sessions_frequency_ = DEFAULT_CLEAN_SESSIONS_FREQUENCY;
          }
        }
        std::string token_length_str(GetProperty("session_token_length"));
        if (token_length_str.empty()){
          token_length_ = DEFAULT_TOKEN_LENGTH;
        }else{
          try{
            token_length_ = std::stoi(token_length_str);
          }catch(const std::exception& e){
            token_length_ = DEFAULT_TOKEN_LENGTH;
          }
        }
      }

      const std::string SharedMapSessionHandler::GetProperty(const std::string& name){
        return granada::util::application::GetProperty(name);
      }

      const bool SharedMapSessionHandler::SessionExists(const std::string& token){
        if (!token.empty()){
          mtx.lock();
          auto it = sessions_->find(token);
          if (it != sessions_->end()){
            mtx.unlock();
            return true;
          }
          mtx.unlock();
        }
        return false;
      }

      const std::string SharedMapSessionHandler::GenerateToken(){
        return SharedMapSessionHandler::n_generator_->generate();
      }

      void SharedMapSessionHandler::LoadSession(const std::string& token,granada::http::session::Session* virgin){
        if (!token.empty()){
          mtx.lock();
          auto it = sessions_->find(token);
          if (it != sessions_->end()){
            granada::http::session::Session* session = it->second.get();
            if (session->IsValid()){
              virgin->set(session);
            }
          }
          mtx.unlock();
        }
      }

      void SharedMapSessionHandler::SaveSession(std::shared_ptr<granada::http::session::Session> session){
        std::string token = session->GetToken();
        if (!token.empty()){
          mtx.lock();
          (*sessions_)[token] = session;
          mtx.unlock();
        }
      }

      void SharedMapSessionHandler::DeleteSession(granada::http::session::Session* session){
        std::string token = session->GetToken();
        if (!token.empty()){
          mtx.lock();
          sessions_->erase(token);
          mtx.unlock();
        }
      }

      void SharedMapSessionHandler::CleanSessions(){
        std::string token;
        std::vector<granada::http::session::Session*> sessions_to_erase;
        mtx.lock();

        // loop throw all sessions.
        for ( auto it = sessions_->begin(); it != sessions_->end(); ++it ){
          token = it->first;
          granada::http::session::Session* session = it->second.get();

          // check if session is garbage.
          if (session->IsGarbage()){
            sessions_to_erase.push_back(session);
          }
        }

        mtx.unlock();

        // erase sessions
        for (auto it = sessions_to_erase.begin(); it != sessions_to_erase.end();++it){
          (*it)->Close();
        }
      }

      void SharedMapSessionHandler::CleanSessions(bool recursive){
        CleanSessions();

        if (clean_sessions_frequency_>-1){
          // wait for x seconds...
          std::this_thread::sleep_for (std::chrono::duration<double>(clean_sessions_frequency_));
          // ... and clean sessions again.
          CleanSessions(true);
        }
      }
    }
  }
}
