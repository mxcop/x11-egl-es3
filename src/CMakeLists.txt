# Glob all source files
file(GLOB_RECURSE SOURCES
	"*.cpp"
)

# Add source files to the program
target_sources(program PRIVATE
	${SOURCES}
)
