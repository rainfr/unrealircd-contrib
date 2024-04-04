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

ModuleHeader MOD_HEADER = {
	"third/m_ipident", // Module name
	"1.0", // Module Version
	"Set consistent ident based on user IP.", // Description
	"musk", // Author
	"unrealircd-6", // Unreal Version
};

// Forward declaration of the hook function
int set_ip_based_ident(Client *client);

// Module Initialization
MOD_INIT() {
    HookAdd(modinfo->handle, HOOKTYPE_LOCAL_CONNECT, 0, set_ip_based_ident);
    return MOD_SUCCESS;
}

// Called when the module is loaded
MOD_LOAD() {
    return MOD_SUCCESS;
}

// Called when the module is unloaded
MOD_UNLOAD() {
    return MOD_SUCCESS;
}

// Simple hash function
unsigned long hash_ip(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

// Hook function to set the ident based on IP hash
int set_ip_based_ident(Client *client) {
    if (!client->ip || !client->user) {
        return HOOK_CONTINUE; // Ensure the user structure and IP are valid
    }

    // Convert the IP address to a string if needed
    // Example assumes `client->ip` is a string. Adjust based on actual data structure.

    char hashIdent[10]; // 9 characters + NULL terminator
    unsigned long ipHash = hash_ip(client->ip); // Hash the IP address
    snprintf(hashIdent, sizeof(hashIdent), "%08lx", ipHash & 0xFFFFFFFF); // Convert hash to a 9-character ident

    // Safely set the user's ident based on hashed IP
    strlcpy(client->user->username, hashIdent, sizeof(client->user->username));

    return HOOK_CONTINUE; // Allow other modules to process
}
