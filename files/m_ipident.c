/*
  Licence: GPLv3 or later
  Copyright Ⓒ 2024 Jean Chevronnet
  
*/
/*** <<<MODULE MANAGER START>>>
module
{
		documentation "https://github.com/rainfr/musk-unrealircd-6-contrib/blob/main/m_ipident/README.md";
		troubleshooting "In case of problems, documentation or e-mail me at mike.chevronnet@gmail.com";
		min-unrealircd-version "6.*";
		max-unrealircd-version "6.*";
		post-install-text {
				"The module is installed. Now all you need to do is add a loadmodule line:";
				"loadmodule \"third/m_ipident\";";
				"And /REHASH the IRCd.";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"
#include <openssl/sha.h>

ModuleHeader MOD_HEADER = {
    "third/m_ipident", 
    "1.0.1",
    "Generate ident based on SHA-256 hash of IP (IPv4 and IPv6 support) and user-defined cloak key", 
    "musk",               
    "unrealircd-6",        
};

int set_crypto_ip_based_ident(Client *client);

MOD_INIT() {
    HookAdd(modinfo->handle, HOOKTYPE_LOCAL_CONNECT, 0, set_crypto_ip_based_ident);
    return MOD_SUCCESS;
}

MOD_LOAD() {
    return MOD_SUCCESS;
}

MOD_UNLOAD() {
    return MOD_SUCCESS;
}


static int is_ipv6_address(const char *ip) {
    return strchr(ip, ':') != NULL;  
}

int set_crypto_ip_based_ident(Client *client) {
    if (!client->ip || !client->user) {
        return HOOK_CONTINUE; 
    }

    int is_ipv6 = is_ipv6_address(client->ip);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    char ident[10];
    if (is_ipv6) {
        
        SHA256((unsigned char*)client->ip, strlen(client->ip), hash);
    } else {
        
        SHA256((unsigned char*)client->ip, strlen(client->ip), hash);
    }

    const char *cloak_keys[] = {

        "yY90gBRfJMSqN45WSLM9ttPQB57cVJbTN3nkDi5ZwGtXwn4pZ9JcJFGNwtJX82W8mBBXzJxUXPxwkMNJaP9fXcrxz7ApihCBp3YUt2TSAWp4TFTRfmQBAvHCc",
        "8ed78KM7yhyS8E2SDrVX9t7c8CYQ2YKcQrVff5Keg9dpp6BgTzPE4Jk9wA99HcMShmwp3ntZnnunuzUBwtJuQqMaXTBD8XuVRg3eVGgGARqxHy4YfYMXEnbxY",
        "RcsG6RXNZZitkdtuhvzGVpY6cHEFdvAWunFnbSvEzJhV3zCrSYG56HiQaT3ES5TFc4YywgaZVxepyQBNWcvtD2U3ddG4rCKanZPjV6TMT4jg6YrbQ4dMvHRit"
    };

    const char *cloak_key = cloak_keys[rand() % (sizeof(cloak_keys) / sizeof(cloak_keys[0]))];
    char combined_hash[SHA256_DIGEST_LENGTH + 64 + 1];
    snprintf(combined_hash, sizeof(combined_hash), "%s%s", client->ip, cloak_key);
    for (int i = 0; i < 9; ++i) {
        unsigned char byte = combined_hash[i % (SHA256_DIGEST_LENGTH + 64)];
        if (i < 6) {
            ident[i] = (byte % 2 == 0) ? 'a' + (byte % 26) : 'A' + (byte % 26);
        } else {
            ident[i] = '0' + (byte % 10);
        }
    }
    ident[9] = '\0';
    strlcpy(client->user->username, ident, sizeof(client->user->username));
    return HOOK_CONTINUE;
}
