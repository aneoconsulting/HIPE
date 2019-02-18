//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#ifndef CRYPTO_HPP
#define	CRYPTO_HPP

#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <istream>
#include <vector>

//Moving these to a seperate namespace for minimal global namespace cluttering does not work with clang++
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

namespace SimpleWeb {
    //TODO 2017: remove workaround for MSVS 2012
    #if _MSC_VER == 1700 //MSVS 2012 has no definition for round()
        inline double round(double x) { //custom definition of round() for positive numbers
            return floor(x + 0.5);
        }
    #endif

    class Crypto {
        const static size_t buffer_size=131072;
    public:
        class Base64 {
        public:
            static std::string encode(const std::string &ascii) {
                std::string base64;
                
                BIO *bio, *b64;
                BUF_MEM *bptr=BUF_MEM_new();

                b64 = BIO_new(BIO_f_base64());
                BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
                bio = BIO_new(BIO_s_mem());
                BIO_push(b64, bio);
                BIO_set_mem_buf(b64, bptr, BIO_CLOSE);
                
                //Write directly to base64-buffer to avoid copy
                int base64_length=static_cast<int>(round(4*ceil((double)ascii.size()/3.0)));
                base64.resize(base64_length);
                bptr->length=0;
                bptr->max=base64_length+1;
                bptr->data=(char*)&base64[0];

                BIO_write(b64, &ascii[0], static_cast<int>(ascii.size()));
                BIO_flush(b64);

                //To keep &base64[0] through BIO_free_all(b64)
                bptr->length=0;
                bptr->max=0;
                bptr->data=nullptr;

                BIO_free_all(b64);
                
                return base64;
            }
            
            static std::string decode(const std::string &base64) {
                std::string ascii;
                
                //Resize ascii, however, the size is a up to two bytes too large.
                ascii.resize((6*base64.size())/8);
                BIO *b64, *bio;

                b64 = BIO_new(BIO_f_base64());
                BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
                bio = BIO_new_mem_buf((char*)&base64[0], static_cast<int>(base64.size()));
                bio = BIO_push(b64, bio);

                int decoded_length = BIO_read(bio, &ascii[0], static_cast<int>(ascii.size()));
                ascii.resize(decoded_length);

                BIO_free_all(b64);
                
                return ascii;
            }
        };
        
        /// Return hex string from bytes in input string.
        static std::string to_hex_string(const std::string &input) {
          std::stringstream hex_stream;
          hex_stream << std::hex << std::internal << std::setfill('0');
          for (auto &byte : input)
            hex_stream << std::setw(2) << static_cast<int>(static_cast<unsigned char>(byte));
          return hex_stream.str();
        }
        
        static std::string md5(const std::string &input, size_t iterations=1) {
            std::string hash;
            
            hash.resize(128 / 8);
            MD5(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            for (size_t c = 1; c < iterations; ++c)
              MD5(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }
        
        static std::string md5(std::istream &stream, size_t iterations=1) {
            MD5_CTX context;
            MD5_Init(&context);
            std::streamsize read_length;
            std::vector<char> buffer(buffer_size);
            while((read_length=stream.read(&buffer[0], buffer_size).gcount())>0)
                MD5_Update(&context, buffer.data(), read_length);
            std::string hash;
            hash.resize(128 / 8);
            MD5_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
            
            for (size_t c = 1; c < iterations; ++c)
              MD5(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }

        static std::string sha1(const std::string &input, size_t iterations=1) {
            std::string hash;
            
            hash.resize(160 / 8);
            SHA1(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            for (size_t c = 1; c < iterations; ++c)
              SHA1(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }
        
        static std::string sha1(std::istream &stream, size_t iterations=1) {
            SHA_CTX context;
            SHA1_Init(&context);
            std::streamsize read_length;
            std::vector<char> buffer(buffer_size);
            while((read_length=stream.read(&buffer[0], buffer_size).gcount())>0)
                SHA1_Update(&context, buffer.data(), read_length);
            std::string hash;
            hash.resize(160 / 8);
            SHA1_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
            
            for (size_t c = 1; c < iterations; ++c)
              SHA1(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }

        static std::string sha256(const std::string &input, size_t iterations=1) {
            std::string hash;
            
            hash.resize(256 / 8);
            SHA256(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            for (size_t c = 1; c < iterations; ++c)
              SHA256(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }
        
        static std::string sha256(std::istream &stream, size_t iterations=1) {
            SHA256_CTX context;
            SHA256_Init(&context);
            std::streamsize read_length;
            std::vector<char> buffer(buffer_size);
            while((read_length=stream.read(&buffer[0], buffer_size).gcount())>0)
                SHA256_Update(&context, buffer.data(), read_length);
            std::string hash;
            hash.resize(256 / 8);
            SHA256_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
            
            for (size_t c = 1; c < iterations; ++c)
              SHA256(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }

        static std::string sha512(const std::string &input, size_t iterations=1) {
            std::string hash;
            
            hash.resize(512 / 8);
            SHA512(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            for (size_t c = 1; c < iterations; ++c)
              SHA512(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }
        
        static std::string sha512(std::istream &stream, size_t iterations=1) {
            SHA512_CTX context;
            SHA512_Init(&context);
            std::streamsize read_length;
            std::vector<char> buffer(buffer_size);
            while((read_length=stream.read(&buffer[0], buffer_size).gcount())>0)
                SHA512_Update(&context, buffer.data(), read_length);
            std::string hash;
            hash.resize(512 / 8);
            SHA512_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
            
            for (size_t c = 1; c < iterations; ++c)
              SHA512(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));
            
            return hash;
        }
        
        /// key_size is number of bytes of the returned key.
        static std::string pbkdf2(const std::string &password, const std::string &salt, int iterations, int key_size) {
          std::string key;
          key.resize(key_size);
          PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.size(),
                                 reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(), iterations,
                                 key_size, reinterpret_cast<unsigned char*>(&key[0]));
          return key;
        }
    };
}
#endif	/* CRYPTO_HPP */

