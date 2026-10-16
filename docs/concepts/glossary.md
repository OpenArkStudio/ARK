# Concepts

## Module

A module groups related application logic around a focused responsibility. It keeps coupling low by exposing functionality to other modules through interfaces (`IOP`, interface-oriented programming). A logging module is one example.

## Plugin

A plugin groups modules that serve a broader area of functionality, such as game rules or navigation.

## Application

An application is an independent process that provides a complete set of functionality. It can contain many plugins; a game server is one example.

## Data node

A data node stores an individual entity property, such as a player's HP or MP. Values can be booleans, integers (including 64-bit integers), single- or double-precision floating-point numbers, strings or GUIDs.

## Data table

A data table represents two-dimensional data attached to an entity, such as a player's item inventory. Like a Microsoft Excel sheet, it contains rows and columns, and its schema can be supplied from Excel. Each cell supports the same value types as a data node.

## Entity

An entity is an object created dynamically in a game. It typically contains multiple data nodes and data tables. Players, monsters and NPCs are examples.

## Application launcher

The application launcher loads plugin shared libraries to form an application.

## GUID

A GUID is a game identifier that is unique across all regions and servers. It can be used for entity identifiers and other values that require this scope of uniqueness.

## Event

Application logic emits and subscribes to events to reduce coupling between its components.

## Google Protocol Buffers

Google's open-source data serialization library supports multiple platforms and languages. See [Protocol Buffers](https://github.com/google/protobuf).
