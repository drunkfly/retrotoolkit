#define LIBSPECTRUM_HAS_GLIB_REPLACEMENT 1

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

typedef char gchar;
typedef int gint;
typedef /*long*/ptrdiff_t glong;
typedef gint gboolean;
typedef unsigned int guint;
typedef /*unsigned long*/size_t gulong;
typedef const void * gconstpointer;
typedef void * gpointer;

typedef struct _GSList GSList;

struct _GSList {
    gpointer data;
    GSList *next;
};

typedef void		(*GFunc)		(gpointer	data,
						 gpointer	user_data);

typedef gint		(*GCompareFunc)		(gconstpointer	a,
						 gconstpointer	b);

typedef void           (*GDestroyNotify)       (gpointer       data);

typedef void           (*GFreeFunc)            (gpointer       data);


WIN32_DLL GSList *g_slist_insert_sorted	(GSList		*list,
						 gpointer	 data,
						 GCompareFunc	 func);

WIN32_DLL GSList *g_slist_insert		(GSList		*list,
			 			 gpointer	 data,
			 			 gint		 position);

WIN32_DLL GSList *g_slist_append		(GSList		*list,
						 gpointer	 data);

WIN32_DLL GSList *g_slist_prepend		(GSList		*list,
						 gpointer	 data);

WIN32_DLL GSList *g_slist_remove		(GSList		*list,
						 gconstpointer	 data);

WIN32_DLL GSList *g_slist_last			(GSList		*list);

WIN32_DLL GSList *g_slist_reverse		(GSList		*list);

WIN32_DLL GSList *g_slist_delete_link		(GSList		*list,
				 		 GSList		*link);

WIN32_DLL guint	g_slist_length		(GSList *list);

WIN32_DLL void	g_slist_foreach			(GSList		*list,
						 GFunc		 func,
						 gpointer	 user_data);

WIN32_DLL void	g_slist_free			(GSList		*list);

WIN32_DLL GSList *g_slist_nth		(GSList		*list,
					 guint		n);

WIN32_DLL GSList *g_slist_find_custom	(GSList		*list,
					 gconstpointer	data,
					 GCompareFunc	func );

WIN32_DLL gint	g_slist_position	(GSList		*list,
					 GSList		*llink);

typedef struct _GHashTable	GHashTable;

typedef guint		(*GHashFunc)		(gconstpointer	key);

typedef void	(*GHFunc)		(gpointer	key,
						 gpointer	value,
						 gpointer	user_data);

typedef gboolean	(*GHRFunc)		(gpointer	key,
						 gpointer	value,
						 gpointer	user_data);

WIN32_DLL gint	g_int_equal (gconstpointer   v,
			       gconstpointer   v2);
WIN32_DLL guint	g_int_hash  (gconstpointer   v);

WIN32_DLL gint	g_str_equal (gconstpointer   v,
			       gconstpointer   v2);
WIN32_DLL guint	g_str_hash  (gconstpointer   v);

WIN32_DLL GHashTable *g_hash_table_new	(GHashFunc	 hash_func,
					 GCompareFunc	 key_compare_func);

WIN32_DLL GHashTable *g_hash_table_new_full (GHashFunc       hash_func,
                                             GCompareFunc    key_equal_func,
                                             GDestroyNotify  key_destroy_func,
                                             GDestroyNotify  value_destroy_func);

WIN32_DLL void	g_hash_table_destroy	(GHashTable	*hash_table);

WIN32_DLL void	g_hash_table_insert	(GHashTable	*hash_table,
					 gpointer	 key,
					 gpointer	 value);

WIN32_DLL gpointer g_hash_table_lookup	(GHashTable	*hash_table,
					 gconstpointer	 key);

WIN32_DLL void	g_hash_table_foreach (GHashTable	*hash_table,
						 GHFunc    func,
						 gpointer  user_data);

WIN32_DLL guint	g_hash_table_foreach_remove	(GHashTable	*hash_table,
						 GHRFunc	 func,
						 gpointer	 user_data);

WIN32_DLL guint	g_hash_table_size (GHashTable	*hash_table);

typedef struct _GArray GArray;

struct _GArray {
  /* Public */
  gchar *data;
  size_t len;

  /* Private */
  guint element_size;
  size_t allocated;
};

WIN32_DLL GArray* g_array_new( gboolean zero_terminated, gboolean clear,
		      guint element_size );
WIN32_DLL GArray* g_array_sized_new( gboolean zero_terminated, gboolean clear,
                   guint element_size, guint reserved_size );
#define g_array_append_val(a,v) g_array_append_vals( a, &(v), 1 );
WIN32_DLL GArray* g_array_append_vals( GArray *array, gconstpointer data, guint len );
#define g_array_index(a,t,i) (*(((t*)a->data)+i))
WIN32_DLL GArray* g_array_set_size( GArray *array, guint length );
WIN32_DLL GArray* g_array_remove_index_fast( GArray *array, guint index );
WIN32_DLL gchar* g_array_free( GArray *array, gboolean free_segment );

#define GINT_TO_POINTER(i)      ((gpointer)  (i))
#define GPOINTER_TO_INT(p)      ((gint)   (p))
#define GPOINTER_TO_UINT(p)     ((guint)  (p))
