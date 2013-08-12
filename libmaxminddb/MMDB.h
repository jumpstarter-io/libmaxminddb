#ifndef MMDB_H
#define MMDB_H

#ifdef __cplusplus
extern "C" {
#endif
#define _GNU_SOURCE
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>

#define MMDB_DEFAULT_DATABASE "/usr/local/share/GeoIP2/GeoIP2-City.mmdb"

// *** the EXT_TYPE is wrong it should be type - 8 not type
#define BROKEN_TYPE (1)

#define MMDB_DTYPE_EXT (0)
#define MMDB_DTYPE_PTR (1)
#define MMDB_DTYPE_UTF8_STRING (2)
#define MMDB_DTYPE_IEEE754_DOUBLE (3)
#define MMDB_DTYPE_BYTES (4)
#define MMDB_DTYPE_UINT16 (5)
#define MMDB_DTYPE_UINT32 (6)
#define MMDB_DTYPE_MAP (7)      /* HASH */
#define MMDB_DTYPE_INT32 (8)
#define MMDB_DTYPE_UINT64 (9)
#define MMDB_DTYPE_UINT128 (10)
#define MMDB_DTYPE_ARRAY (11)
#define MMDB_DTYPE_CONTAINER (12)
#define MMDB_DTYPE_END_MARKER (13)
#define MMDB_DTYPE_BOOLEAN (14)
#define MMDB_DTYPE_IEEE754_FLOAT (15)

#define MMDB_DTYPE_MAX (MMDB_DTYPE_IEEE754_FLOAT)

#define MMDB_DATASECTION_NOOP_SIZE (16)

/* GEOIPDB flags */
#define MMDB_MODE_STANDARD (1)
#define MMDB_MODE_MEMORY_CACHE (2)
#define MMDB_MODE_MASK (7)

/* GEOIPDB err codes */
#define MMDB_SUCCESS (0)
#define MMDB_FILE_OPEN_ERROR (1)
#define MMDB_CORRUPT_DATABASE (2)
#define MMDB_INVALID_DATABASE (3)
#define MMDB_IO_ERROR (4)
#define MMDB_OUT_OF_MEMORY (5)
#define MMDB_UNKNOWN_DATABASE_FORMAT (6)

/* Looks better */
#define MMDB_TRUE (1)
#define MMDB_FALSE (0)

/* */
#define MMDB_DEBUG (0)

#if MMDB_DEBUG
#define MMDB_DBG_CARP(...) fprintf(stderr, __VA_ARGS__ );
#define MMDB_DBG_ASSERT(ex) assert(#ex)
#else
#define MMDB_DBG_CARP(...)
#define MMDB_DBG_ASSERT(ex)
#endif
    // This is the starting point for every search.
    // It is like the hash to start the search. It may or may not the root hash
    typedef struct MMDB_entry_s {
        struct MMDB_s *mmdb;
        unsigned int offset;    /* A pointer to the start of the data for an IP */
    } MMDB_entry_s;

    // This is a pointer to the first 
    // think of it as the root of all informations about the IP.
    typedef struct {
        MMDB_entry_s entry;
        int netmask;
    } MMDB_root_entry_s;

    // this is the result for every field
    typedef struct MMDB_return_s {
        /* return values */
        union {
            float float_value;
            double double_value;
            int sinteger;
            uint32_t uinteger;
            uint8_t c8[8];
            uint8_t c16[16];
            const void *ptr;
        };
        uint32_t offset;        /* start of our field or zero for not found */
        int data_size;          /* only valid for strings, utf8_strings or binary data */
        int type;               /* type like string utf8_string, int32, ... */
    } MMDB_return_s;

    typedef struct MMDB_description_s {
        const char *language;
        const char *description;
    } MMDB_description_s;

    typedef struct MMDB_metadata_s {
        int node_count;
        int record_size;
        int ip_version;
        char *database_type;
        struct {
            size_t count;
            const char **names;
        } languages;
        int binary_format_major_version;
        int binary_format_minor_version;
        unsigned long long build_epoch;
        struct {
            size_t count;
            MMDB_description_s **descriptions;
        } description;
    } MMDB_metadata_s;

    typedef struct MMDB_s {
        uint32_t flags;
        int fd;
        char *fname;
        const uint8_t *file_in_mem_ptr;
        const uint8_t *dataptr;
        uint32_t full_record_byte_size;
        int depth;
        MMDB_metadata_s metadata;
        struct MMDB_s *fake_metadata_db;
        MMDB_entry_s meta;      // should change to entry_s
    } MMDB_s;

    // The decode structure is like the result ( return_s ) but with the start
    // of the next entry. For example if we search for a key but this is the
    // wrong key.
    typedef struct MMDB_decode_s {
        MMDB_return_s data;
        uint32_t offset_to_next;
    } MMDB_decode_s;

    typedef struct MMDB_decode_all_s {
        MMDB_decode_s decode;
        struct MMDB_decode_all_s *next;
    } MMDB_decode_all_s;

    extern uint16_t MMDB_open(const char *fname, uint32_t flags, MMDB_s * mmdb);
    extern void MMDB_close(MMDB_s * mmdb);
    extern int MMDB_lookup_by_ipnum(uint32_t ipnum, MMDB_root_entry_s * res);
    extern int MMDB_lookup_by_ipnum_128(struct in6_addr ipnum,
                                        MMDB_root_entry_s * result);

    extern int MMDB_get_value(MMDB_entry_s * start, MMDB_return_s * result,
                              ...);
    extern int MMDB_strcmp_result(MMDB_s * mmdb,
                                  MMDB_return_s const *const result, char *str);

    extern const char *MMDB_lib_version(void);

    extern int MMDB_dump(MMDB_s * mmdb, MMDB_decode_all_s * decode_all,
                         int indent);
    extern void MMDB_get_tree(MMDB_entry_s * start,
                              MMDB_decode_all_s ** decode_all);
    extern MMDB_decode_all_s *MMDB_alloc_decode_all(void);
    extern void MMDB_free_decode_all(MMDB_decode_all_s * freeme);

    extern int MMDB_resolve_address(const char *host, int ai_family,
                                    int ai_flags, void *ip);

    extern MMDB_root_entry_s *MMDB_lookup(MMDB_s * mmdb, const char *ipstr,
                                          int *gai_error, int *mmdb_error);

    extern int MMDB_pread(int fd, uint8_t * buffer, ssize_t to_read,
                          off_t offset);
#ifdef __cplusplus
}
#endif
#endif                          /* MMDB_H */
