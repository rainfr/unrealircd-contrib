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
				"Please see the README for operclass requirements";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"
#include <openssl/sha.h>

ModuleHeader MOD_HEADER = {
    "third/m_ipident", /* Replace with your module name */
    "1.0",                   /* Module version */
    "Generate ident based on SHA-256 hash of IP", /* Short description of module */
    "musk",               /* Author */
    "unrealircd-6",        /* Unreal Version */
};

/* Function declarations */
int set_crypto_ip_based_ident(Client *client);

/* Initializes the module */
MOD_INIT() {
    HookAdd(modinfo->handle, HOOKTYPE_LOCAL_CONNECT, 0, set_crypto_ip_based_ident);
    return MOD_SUCCESS;
}

/* Loads the module */
MOD_LOAD() {
    return MOD_SUCCESS;
}

/* Unloads the module */
MOD_UNLOAD() {
    return MOD_SUCCESS;
}

/* This function generates an ident based on SHA-256 hash of the user's IP */
int set_crypto_ip_based_ident(Client *client) {
    if (!client->ip || !client->user) {
        return HOOK_CONTINUE; // Ensure the client structure is valid.
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    char hashIdent[10]; // Ident will be 9 characters + NULL terminator.
    SHA256((unsigned char*)client->ip, strlen(client->ip), hash); // Generate SHA-256 hash of IP address.

    // Convert the first few bytes of the hash into a base36-like ident (a-z0-9).
    for (int i = 0; i < 9; ++i) {
        unsigned char byte = hash[i % SHA256_DIGEST_LENGTH];
        if (i < 6) {
            // Map the first 6 bytes to [a-z]
            hashIdent[i] = 'a' + (byte % 26);
        } else {
            // Map the next 3 bytes to [0-9]
            hashIdent[i] = '0' + (byte % 10);
        }
    }
    hashIdent[9] = '\0'; // Null-terminate the ident string.

    // Set the user's ident
    strlcpy(client->user->username, hashIdent, sizeof(client->user->username));

    return HOOK_CONTINUE;
}
