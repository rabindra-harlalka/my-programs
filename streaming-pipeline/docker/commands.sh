docker build -t httpd-flume-avro-agent .
docker build -f Dockerfile_avrosrc -t ubuntu-avrosrc .
docker build -f Dockerfile_avro -t http-flume-logging .
docker build -f Dockerfile_pyspark -t pyspark .

docker network create spa-net
docker run -d --rm --net spa-net --name flume_agent -p 10000:4141 -it ubuntu-avrosrc
docker run -d --rm -e TZ=UTC --net spa-net --name apache2_logger -p 8080:80 http-flume-logging

docker run -d --rm -p 2181:2181 --name zookeeper --net spa-net ubuntu/zookeeper
docker run -d --rm -p 9092:9092 --name kafka --net spa-net -e TZ=UTC -e ZOOKEEPER_HOST=zookeeper ubuntu/kafka

docker run --rm -it --name pyspark --net spa-net pyspark
