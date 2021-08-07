idf_component_register(SRCS "main.c" "file_server.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES "favicon.ico" "upload_script.html"
                    EMBED_TXTFILES "certs/cacert.pem"
                                   "certs/prvtkey.pem")

                                #    REQUIRES esp_https_server spiffs nvs_flash protocol_examples_common