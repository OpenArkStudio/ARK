CREATE DATABASE photon_oss;

CREATE TABLE `photon_oss`.`player_change_operation` (
	`type` Int32,
	`base_server_id` String,
	`base_event_time` DateTime,
	`player_open_id` String,
	`player_player_name` String
) ENGINE = ReplicatedReplacingMergeTree('/clickhouse/tables/{shard}/photon_oss.player_change_operation', '{replica}')
PARTITION BY `base_event_time`
ORDER BY (event_time)
SETTINGS index_granularity = 8192;

CREATE TABLE `photon_oss`.`player_change_operation_all` AS `photon_oss`.`player_change_operation` ENGINE = Distributed(xdoss, photon_oss, player_change_operation, rand());

CREATE TABLE `photon_oss`.`room_count_change` (
	`type` Int32,
	`base_server_id` String,
	`base_event_time` DateTime,
	`old_computer_open_id` String,
	`old_computer_player_name` String,
	`new_computer_open_id` String,
	`new_computer_player_name` String
) ENGINE = ReplicatedReplacingMergeTree('/clickhouse/tables/{shard}/photon_oss.room_count_change', '{replica}')
PARTITION BY `base_event_time`
ORDER BY (event_time)
SETTINGS index_granularity = 8192;

CREATE TABLE `photon_oss`.`room_count_change_all` AS `photon_oss`.`room_count_change` ENGINE = Distributed(xdoss, photon_oss, room_count_change, rand());
