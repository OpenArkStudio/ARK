# --build-arg 来设置参数
# From the repository root: docker build -t ark:escort -f deployment/Dockerfile .

FROM docker/ark-alpine-build:latest AS Builder

WORKDIR /server/escort

COPY . /server/escort

RUN cd ./src/proto \
    #&& sh ./gen-proto.sh \
    && cd ../../ \
    && rm -rf ./cmake-build | true \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DARK_OPEN_FILE_LOG=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_EXAMPLES=OFF .. \
    && make -j16 \
    && cd ../ \
    && mkdir -p /home/server/escort \
    && cp -r ./cmake-build/bin /home/server/escort \
    && cp -r ./build/conf /home/server/escort

FROM  docker/ark-alpine-build:latest

WORKDIR /home/server/escort

#COPY --from=Builder /usr/lib/x86_64-linux-gnu/librdkafka++.so.1 /usr/lib/x86_64-linux-gnu/librdkafka++.so.1
#COPY --from=Builder /usr/lib/x86_64-linux-gnu/librdkafka.so.1 /usr/lib/x86_64-linux-gnu/librdkafka.so.1
COPY --from=Builder /home/server/escort /home/server/escort

