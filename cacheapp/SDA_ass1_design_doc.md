# Design document

## Overview
Implemented in .NET core C# language. Instructions for compiling and running the program are in a separate document.

The program takes a batch.txt file as input containing one more more lines which have the file names of the json files storing the MongoDB query. The queries will be executed in the same order as the files appear in the batch file.

Program will print whether it's a hit or miss, the time taken, the no. of records returned, whether eviction took place, and if yes, which/how many items were evicted from the caches.

## Query parsing/translation
We parse the incoming json query into a expression tree and a set of projected attributes. As soon as a query is executed on MongoDB cloud, we store it in the query cache.
To check if a subsequent query is covered by a cached query, we check if the cached query is identical to the new query or borader in scope than the new query. If yes, we consider it a hit. Otherwise, it's a miss.

## Cache Design
We use LiteDB, a library that providers local persistent database and doesn't require any installation or configuration upfront. LiteDB is a non-SQL database.

We have two caches - `a query cache` and `a result cache`.

<tables for illustration>

The `Query cache` table : 

| QueryID       |   Query       |   Timestamp   |
| ------------- | ------------- | ------------- |
| Content Cell  | Content Cell  | Content Cell  |
| Content Cell  | Content Cell  | Content Cell  |

The `Results cache` table : 

|      ID       |   QueryID     |   Timestamp   | Collections   |
| ------------- | ------------- | ------------- | ------------- |
| Content Cell  | Content Cell  | Content Cell  | Content Cell  |
| Content Cell  | Content Cell  | Content Cell  | Content Cell  |

<explanation for illustration>





### Why we used LiteDB and SQLite (a SQL database)
Since queries can request for variable numbers of attributes to project, we can't predict how many attributes will be needed overall to store the results. To overcome this, we either need to expand our cache table definition from time to time whenever a new query has additional attributes not seen before, or we need something more than plain SQL to match the result json stored in a single attribute.

So, we are using LiteDB to be able to store result of any shape (any no. of attributes) and keep our caching solution general-purpose. In other words, unlike SQL table, we don't have to know upfront that we are serving airbnd collections and then make our result cache table with all the possible attributes.

## LRU cache

**Timestamp**

- query cache collection
- result cache collection

Result cache takes more space, so it's imperative to remove the results, not just the much lighter query, that have not been recently accessed from the cache.

We have implemented min-heap for LRU where it's keyed by timestamp.
