#!/bin/sh

HCOLOR='\033[0;36m'
NC='\033[0m' # No Color

TIME_EXEC="/bin/time -p"
TEST_FILES_DIR="./test-files"
EXEC_NAME="./zad1"
REC_SIZES="4"
REC_SIZES="${REC_SIZES} 512"
REC_SIZES="${REC_SIZES} 4096"
REC_SIZES="${REC_SIZES} 8192"
REC_COUNTS="1000"
REC_COUNTS="${REC_COUNTS} 2000"

if [ ! -f "${EXEC_NAME}" ]; then
    echo "Executable not found. Have you compiled it already?"
    exit 1
fi

if [ ! -d "${TEST_FILES_DIR}" ]; then
    echo "Creating tests directory..."
    mkdir ${TEST_FILES_DIR}
fi

for COUNT in ${REC_COUNTS}; do
    for SIZE in ${REC_SIZES}; do
        echo -e "${HCOLOR}====[Testing with record count ${COUNT} and size ${SIZE} bytes]====${NC}"
        echo "=> Generating test files..."
        TEST_FILE_SYS="${TEST_FILES_DIR}/test-sys-${COUNT}-${SIZE}b.bin"
        TEST_FILE_LIB="${TEST_FILES_DIR}/test-lib-${COUNT}-${SIZE}b.bin"
        eval "${EXEC_NAME} sys generate ${TEST_FILE_SYS} ${COUNT} ${SIZE}"
        eval "cp ${TEST_FILE_SYS} ${TEST_FILE_LIB}"

        echo -e "${HCOLOR}=> Sorting tests...${NC}"
        echo "==> System library"
        eval "${TIME_EXEC} ${EXEC_NAME} sys sort ${TEST_FILE_SYS} ${COUNT} ${SIZE}"
        echo -e "\n==>C library"
        eval "${TIME_EXEC} ${EXEC_NAME} lib sort ${TEST_FILE_LIB} ${COUNT} ${SIZE}"

        echo -e "\n${HCOLOR}=> Shuffle tests...${NC}"
        echo "==> System library"
        eval "${TIME_EXEC} ${EXEC_NAME} sys sort ${TEST_FILE_SYS} ${COUNT} ${SIZE}"
        echo -e "\n==> C library"
        eval "${TIME_EXEC} ${EXEC_NAME} lib sort ${TEST_FILE_LIB} ${COUNT} ${SIZE}"

        echo -e "\n${HCOLOR}====[END]====${NC}\n\n"
    done
done
