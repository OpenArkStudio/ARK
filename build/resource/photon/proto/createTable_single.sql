CREATE DATABASE photon_oss;

CREATE TABLE `photon_oss`.`player_change_operation_all` (
	`type` Int32,
	`base_server_id` String,
	`base_event_time` DateTime,
	`player_open_id` String,
	`player_player_name` String
) ENGINE = ReplacingMergeTree()
PARTITION BY `base_event_time`
ORDER BY (event_time)
SETTINGS index_granularity = 8192;

CREATE TABLE `photon_oss`.`room_count_change_all` (
	`type` Int32,
	`base_server_id` String,
	`base_event_time` DateTime,
	`old_computer_open_id` String,
	`old_computer_player_name` String,
	`new_computer_open_id` String,
	`new_computer_player_name` String
) ENGINE = ReplacingMergeTree()
PARTITION BY `base_event_time`
ORDER BY (event_time)
SETTINGS index_granularity = 8192;
