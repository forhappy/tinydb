#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <evhtp.h>

static int
output_header(evhtp_header_t * header, void * arg) {
    evbuf_t * buf = arg;

    evbuffer_add_printf(buf, "print_kvs() key = '%s', val = '%s'\n",
                        header->key, header->val);
    return 0;
}

static evhtp_res
print_kvs(evhtp_request_t * req, evhtp_headers_t * hdrs, void * arg ) {
    evhtp_headers_for_each(hdrs, output_header, req->buffer_out);
    return EVHTP_RES_OK;
}

void
testcb(evhtp_request_t * req, void * a) {
    const char * str = "{\"hello\": \"world\"}";
	int method= evhtp_request_get_method(req);
	evhtp_uri_t *uri = req->uri;
	evhtp_query_t *uri_query = uri->query;

	print_kvs(req, uri_query, NULL);
    evbuffer_add_printf(req->buffer_out, "%s\n%d\n", str, method);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

int
main(int argc, char ** argv) {
    evbase_t         * evbase = event_base_new();
    evhtp_t          * htp    = evhtp_new(evbase, NULL);
    evhtp_callback_t * cb   = NULL;

    cb = evhtp_set_cb(htp, "/metasearch/attr_q", testcb, NULL);
#ifndef EVHTP_DISABLE_EVTHR
    evhtp_use_threads(htp, NULL, 4, NULL);
#endif
    evhtp_bind_socket(htp, "0.0.0.0", 8081, 1024);

    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_callback_free(cb);
    evhtp_free(htp);
    event_base_free(evbase);

    return 0;
}

