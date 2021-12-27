using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using MongoDBQueryCache;

namespace MongoDBCacheApp
{
    class Program
    {
        private static IMongoDBProxy _mongoDbProxy;

        private static readonly string _mongoDBConnectionString = "mongodb+srv://mongodb-010:32EE9xuVdBOhSyRU@sda-group-010.7x6rk.mongodb.net/sample_airbnb?retryWrites=true&w=majority";
        private static readonly string _databaseName = "sample_airbnb";
        private static readonly string _collectionName = "listingsAndReviews";

        static async Task Main(string[] args)
        {
            if (args.Length < 1)
            {
                await Console.Error.WriteLineAsync("Run with the path of input queries file");
                return;
            }

            var batch = new List<string>();
            var fileExtension = Path.GetExtension(args[0]);
            switch (fileExtension)
            {
                case ".txt":
                {
                    var directory = Path.GetDirectoryName(args[0]);
                    batch.AddRange(File.ReadLines(args[0])
                        .Where(line => line.EndsWith(".json"))
                        .Select(line => Path.Combine(directory!, line))
                        .Where(File.Exists));
                    break;
                }
                case ".json":
                    batch.Add(args[0]);
                    break;
                default:
                    await Console.Error.WriteLineAsync("unrecognized input file");
                    return;
            }

            _mongoDbProxy = new CachingMongoDbProxy(_mongoDBConnectionString, _databaseName, _collectionName, 3, 5000);

            foreach (var query in batch)
            {
                await RunQuery(query);
            }

            _mongoDbProxy.Close();
        }

        private static async Task RunQuery(string queryJsonFilePath)
        {
            Console.WriteLine("Reading the input queries file {0}", queryJsonFilePath);

            var queryString = await File.ReadAllTextAsync(queryJsonFilePath);

            var stopwatch = Stopwatch.StartNew();
            var count = 0;
            await foreach (var result in _mongoDbProxy.ExecQueryAsync(queryString))
            {
                count++;
                //Console.WriteLine($"{++count}: {result}");
            }
            stopwatch.Stop();
            Console.WriteLine($"Query returned {count} items.");
            Console.WriteLine($"Time elapsed: {stopwatch.ElapsedMilliseconds} ms.");
            Console.WriteLine(new string('=', 80));
        }
    }
}
