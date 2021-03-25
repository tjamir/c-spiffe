#include "../../workload/src/jwtwatcher.h"

void print_function(jwtbundle_Set *set, void *not_used)
{
    BIO *out;
    out = BIO_new_fd(fileno(stdout), BIO_NOCLOSE);

    for(uint32_t i = 0, size = jwtbundle_Set_Len(set); i < size; ++i) {
        printf("TD name: %s\n", set->bundles[i].value->td.name);

        for(size_t j = 0, size2 = hmlenu(set->bundles[i].value->auths);
            j < size2; ++j) {
            printf(" kID: %s\n", set->bundles[i].value->auths[j].key);
            EVP_PKEY_print_params(out, set->bundles[i].value->auths[j].value,
                                  2, NULL);
            EVP_PKEY_print_public(out, set->bundles[i].value->auths[j].value,
                                  2, NULL);
        }
    }
    BIO_free(out);
}

int main(int argc, char const *argv[])
{
    workloadapi_JWTWatcherConfig config;
    config.client = NULL;
    config.client_options = NULL;
    arrpush(config.client_options, workloadapi_Client_defaultOptions);

    workloadapi_JWTCallback cb;
    cb.args = NULL;
    cb.func = print_function;

    err_t error = NO_ERROR;

    workloadapi_JWTWatcher *watcher
        = workloadapi_newJWTWatcher(config, cb, &error);

    if(error) {
        printf("error %d on newJWTWatcher()\n", error);
    }
    printf("press Enter to stop.\n");
    error = workloadapi_JWTWatcher_Start(watcher);

    if(error) {
        printf("error %d on JWTWatcher_Start()\n", error);
    }
    char ch;
    scanf("%c", &ch);

    printf("Stopping.\n");

    error = workloadapi_JWTWatcher_Close(watcher);
    if(error) {
        printf("error %d on JWTWatcher_Close()\n", error);
    }
    error = workloadapi_JWTWatcher_Free(watcher);
    if(error) {
        printf("error %d on JWTWatcher_Free()\n", error);
    }

    arrfree(config.client_options);

    return 0;
}