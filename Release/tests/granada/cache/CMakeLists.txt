set(SOURCES
	${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cpp
	shared_map_cache_driver_test.cpp
)

add_casablanca_test(${LIB}granada_cache_test SOURCES)