set(SOURCES
	${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cc
	shared_map_cache_driver.cc
)

add_casablanca_test(${LIB}granada_cache_test SOURCES)