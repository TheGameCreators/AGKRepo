# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := AGKCurl
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../common/include \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../common/Collision \
    $(LOCAL_PATH)/libpng \
    $(LOCAL_PATH)/libjpeg \
    $(LOCAL_PATH)/../../../zlib \
    $(LOCAL_PATH)/../../../zxing \
    $(LOCAL_PATH)/../../../curl \
    $(LOCAL_PATH)/../../../curl/include \
    $(LOCAL_PATH)/../../../giflib \
    $(LOCAL_PATH)/../../../json \
    $(LOCAL_PATH)/../../../assimp \
    $(LOCAL_PATH)/../../../bullet \
    $(LOCAL_PATH)/../../../bullet/BulletCollision/CollisionShapes
LOCAL_SRC_FILES := ../../../curl/amigaos.c \
    ../../../curl/asyn-ares.c \
    ../../../curl/asyn-thread.c \
    ../../../curl/base64.c \
    ../../../curl/conncache.c \
    ../../../curl/connect.c \
    ../../../curl/content_encoding.c \
    ../../../curl/cookie.c \
    ../../../curl/curl_addrinfo.c \
    ../../../curl/curl_des.c \
    ../../../curl/curl_endian.c \
    ../../../curl/curl_fnmatch.c \
    ../../../curl/curl_gethostname.c \
    ../../../curl/curl_gssapi.c \
    ../../../curl/curl_memrchr.c \
    ../../../curl/curl_multibyte.c \
    ../../../curl/curl_ntlm_core.c \
    ../../../curl/curl_ntlm_wb.c \
    ../../../curl/curl_rtmp.c \
    ../../../curl/curl_sasl.c \
    ../../../curl/curl_sspi.c \
    ../../../curl/curl_threads.c \
    ../../../curl/dict.c \
    ../../../curl/dotdot.c \
    ../../../curl/easy.c \
    ../../../curl/escape.c \
    ../../../curl/file.c \
    ../../../curl/fileinfo.c \
    ../../../curl/formdata.c \
    ../../../curl/ftp.c \
    ../../../curl/ftplistparser.c \
    ../../../curl/getenv.c \
    ../../../curl/getinfo.c \
    ../../../curl/gopher.c \
    ../../../curl/hash.c \
    ../../../curl/hmac.c \
    ../../../curl/hostasyn.c \
    ../../../curl/hostcheck.c \
    ../../../curl/hostip.c \
    ../../../curl/hostip4.c \
    ../../../curl/hostip6.c \
    ../../../curl/hostsyn.c \
    ../../../curl/http.c \
    ../../../curl/http_chunks.c \
    ../../../curl/http_digest.c \
    ../../../curl/http_negotiate.c \
    ../../../curl/http_ntlm.c \
    ../../../curl/http_proxy.c \
    ../../../curl/http2.c \
    ../../../curl/idn_win32.c \
    ../../../curl/if2ip.c \
    ../../../curl/inet_ntop.c \
    ../../../curl/inet_pton.c \
    ../../../curl/ldap.c \
    ../../../curl/llist.c \
    ../../../curl/md4.c \
    ../../../curl/memdebug.c \
    ../../../curl/multi.c \
    ../../../curl/netrc.c \
    ../../../curl/non-ascii.c \
    ../../../curl/nonblock.c \
    ../../../curl/nwlib.c \
    ../../../curl/nwos.c \
    ../../../curl/openldap.c \
    ../../../curl/parsedate.c \
    ../../../curl/pingpong.c \
    ../../../curl/pipeline.c \
    ../../../curl/progress.c \
    ../../../curl/rand.c \
    ../../../curl/rtsp.c \
    ../../../curl/select.c \
    ../../../curl/sendf.c \
    ../../../curl/share.c \
    ../../../curl/slist.c \
    ../../../curl/smb.c \
    ../../../curl/socks.c \
    ../../../curl/socks_gssapi.c \
    ../../../curl/socks_sspi.c \
    ../../../curl/speedcheck.c \
    ../../../curl/splay.c \
    ../../../curl/ssh.c \
    ../../../curl/strdup.c \
    ../../../curl/strcase.c \
    ../../../curl/strerror.c \
    ../../../curl/strtok.c \
    ../../../curl/strtoofft.c \
    ../../../curl/system_win32.c \
    ../../../curl/telnet.c \
    ../../../curl/tftp.c \
    ../../../curl/timeval.c \
    ../../../curl/transfer.c \
    ../../../curl/url.c \
    ../../../curl/warnless.c \
    ../../../curl/wildcard.c \
    ../../../curl/x509asn1.c \
    ../../../curl/vtls/axtls.c \
    ../../../curl/vtls/cyassl.c \
    ../../../curl/vtls/darwinssl.c \
    ../../../curl/vtls/gskit.c \
    ../../../curl/vtls/gtls.c \
    ../../../curl/vtls/mbedtls.c \
    ../../../curl/vtls/nss.c \
    ../../../curl/vtls/openssl.c \
    ../../../curl/vtls/polarssl.c \
    ../../../curl/vtls/polarssl_threadlock.c \
    ../../../curl/vtls/schannel.c \
    ../../../curl/vtls/vtls.c \
    ../../../curl/imap.c \
    ../../../curl/mprintf.c \
    ../../../curl/md5.c \
    ../../../curl/pop3.c \
    ../../../curl/smtp.c \
    ../../../curl/version.c \
    ../../../curl/mbedTLS/xtea.c \
    ../../../curl/mbedTLS/aes.c \
    ../../../curl/mbedTLS/aesni.c \
    ../../../curl/mbedTLS/arc4.c \
    ../../../curl/mbedTLS/aria.c \
    ../../../curl/mbedTLS/asn1parse.c \
    ../../../curl/mbedTLS/asn1write.c \
    ../../../curl/mbedTLS/base64.c \
    ../../../curl/mbedTLS/bignum.c \
    ../../../curl/mbedTLS/blowfish.c \
    ../../../curl/mbedTLS/camellia.c \
    ../../../curl/mbedTLS/ccm.c \
    ../../../curl/mbedTLS/certs.c \
    ../../../curl/mbedTLS/chacha20.c \
    ../../../curl/mbedTLS/chachapoly.c \
    ../../../curl/mbedTLS/cipher.c \
    ../../../curl/mbedTLS/cipher_wrap.c \
    ../../../curl/mbedTLS/cmac.c \
    ../../../curl/mbedTLS/ctr_drbg.c \
    ../../../curl/mbedTLS/debug.c \
    ../../../curl/mbedTLS/des.c \
    ../../../curl/mbedTLS/dhm.c \
    ../../../curl/mbedTLS/ecdh.c \
    ../../../curl/mbedTLS/ecdsa.c \
    ../../../curl/mbedTLS/ecjpake.c \
    ../../../curl/mbedTLS/ecp.c \
    ../../../curl/mbedTLS/ecp_curves.c \
    ../../../curl/mbedTLS/entropy.c \
    ../../../curl/mbedTLS/entropy_poll.c \
    ../../../curl/mbedTLS/error.c \
    ../../../curl/mbedTLS/gcm.c \
    ../../../curl/mbedTLS/havege.c \
    ../../../curl/mbedTLS/hkdf.c \
    ../../../curl/mbedTLS/hmac_drbg.c \
    ../../../curl/mbedTLS/md.c \
    ../../../curl/mbedTLS/md_wrap.c \
    ../../../curl/mbedTLS/md2.c \
    ../../../curl/mbedTLS/md4.c \
    ../../../curl/mbedTLS/md5.c \
    ../../../curl/mbedTLS/memory_buffer_alloc.c \
    ../../../curl/mbedTLS/net_sockets.c \
    ../../../curl/mbedTLS/nist_kw.c \
    ../../../curl/mbedTLS/oid.c \
    ../../../curl/mbedTLS/padlock.c \
    ../../../curl/mbedTLS/pem.c \
    ../../../curl/mbedTLS/pk.c \
    ../../../curl/mbedTLS/pk_wrap.c \
    ../../../curl/mbedTLS/pkcs5.c \
    ../../../curl/mbedTLS/pkcs11.c \
    ../../../curl/mbedTLS/pkcs12.c \
    ../../../curl/mbedTLS/pkparse.c \
    ../../../curl/mbedTLS/pkwrite.c \
    ../../../curl/mbedTLS/platform.c \
    ../../../curl/mbedTLS/platform_util.c \
    ../../../curl/mbedTLS/poly1305.c \
    ../../../curl/mbedTLS/ripemd160.c \
    ../../../curl/mbedTLS/rsa.c \
    ../../../curl/mbedTLS/rsa_internal.c \
    ../../../curl/mbedTLS/sha1.c \
    ../../../curl/mbedTLS/sha256.c \
    ../../../curl/mbedTLS/sha512.c \
    ../../../curl/mbedTLS/ssl_cache.c \
    ../../../curl/mbedTLS/ssl_ciphersuites.c \
    ../../../curl/mbedTLS/ssl_cli.c \
    ../../../curl/mbedTLS/ssl_cookie.c \
    ../../../curl/mbedTLS/ssl_srv.c \
    ../../../curl/mbedTLS/ssl_ticket.c \
    ../../../curl/mbedTLS/ssl_tls.c \
    ../../../curl/mbedTLS/threading.c \
    ../../../curl/mbedTLS/timing.c \
    ../../../curl/mbedTLS/version.c \
    ../../../curl/mbedTLS/version_features.c \
    ../../../curl/mbedTLS/x509.c \
    ../../../curl/mbedTLS/x509_create.c \
    ../../../curl/mbedTLS/x509_crl.c \
    ../../../curl/mbedTLS/x509_crt.c \
    ../../../curl/mbedTLS/x509_csr.c \
    ../../../curl/mbedTLS/x509write_crt.c \
    ../../../curl/mbedTLS/x509write_csr.c \
    ../../../curl/vauth/cleartext.c \
    ../../../curl/vauth/cram.c \
    ../../../curl/vauth/digest.c \
    ../../../curl/vauth/digest_sspi.c \
    ../../../curl/vauth/krb5_gssapi.c \
    ../../../curl/vauth/krb5_sspi.c \
    ../../../curl/vauth/ntlm.c \
    ../../../curl/vauth/ntlm_sspi.c \
    ../../../curl/vauth/oauth2.c \
    ../../../curl/vauth/spnego_gssapi.c \
    ../../../curl/vauth/spnego_sspi.c \
    ../../../curl/vauth/vauth.c

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES 
LOCAL_CFLAGS += -DIDE_ANDROID -O3 -DUSE_MBEDTLS -DBUILDING_LIBCURL -fsigned-char
LOCAL_CPPFLAGS += -fexceptions -std=c++11
LOCAL_ARM_MODE := arm
LOCAL_SHORT_COMMANDS := false

# hack to avoid compilation output jumping out of the armeabi folder due to ../../../
ifeq ($(NDK_APP_OPTIM),debug)
TARGET_OBJS := $(TARGET_OUT)/objs-debug/objs2
else
TARGET_OBJS := $(TARGET_OUT)/objs/objs2
endif

include $(BUILD_STATIC_LIBRARY)
