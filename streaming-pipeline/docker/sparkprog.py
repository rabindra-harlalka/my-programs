### sparkprog.py
# https://stackoverflow.com/a/70269614
# run as: `spark-submit --packages org.apache.spark:spark-sql-kafka-0-10_2.12:3.3.0 sparkprog.py`
#######

# https://www.rittmanmead.com/blog/2017/01/getting-started-with-spark-streaming-with-python-and-kafka/
#from pyspark import SparkContext
#from pyspark.streaming import StreamingContext
#from pyspark.streaming.kafka import KafkaUtils
#import json
from pyspark.sql import SparkSession

# https://sparkbyexamples.com/pyspark/pyspark-what-is-sparksession/
spark = SparkSession.builder.master("local") \
    .appName('httpd_access_log_agg') \
    .getOrCreate()
sc = spark.sparkContext
sc.setLogLevel('WARN')

df = spark \
  .readStream \
  .format("kafka") \
  .option("kafka.bootstrap.servers", "kafka:9092") \
  .option("subscribe", "httpd_access_log") \
  .option("startingOffsets", "earliest") \
  .load()
#  .option("includeHeaders", "true") \
#df.selectExpr("CAST(key AS STRING)", "CAST(value AS STRING)")

# https://stackoverflow.com/a/41492614
df.writeStream.format("console").start().awaitTermination()