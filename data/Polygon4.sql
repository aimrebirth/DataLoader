CREATE TABLE "Maps" (
"id" INTEGER NOT NULL,
"text_id" TEXT NOT NULL,
"resource" TEXT NOT NULL,
"name_id" INTEGER,
"h_min" REAL,
"h_max" REAL,
"kx" REAL,
"bx" REAL,
"ky" REAL,
"by" REAL,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "Strings" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"table_id" INTEGER,
"ru" TEXT,
"en" TEXT,
PRIMARY KEY ("id") ,
FOREIGN KEY ("table_id") REFERENCES "Tables" ("id")
);

CREATE TABLE "Weapons" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"projectile_id" INTEGER,
"type" INTEGER,
"standard" INTEGER,
"weight" REAL,
"power" REAL,
"firerate" REAL,
"damage" REAL,
"price" REAL,
"fx" REAL,
"shoottype" INTEGER,
"shootscale" REAL,
"xstate" INTEGER,
"rcolor" REAL,
"gcolor" REAL,
"bcolor" REAL,
"typearms" INTEGER,
"tfire" REAL,
"vtype" INTEGER,
"spare" REAL,
"reconstruction" REAL,
"maxdistance" REAL,
"angle" REAL,
"fxtime" REAL,
"damagetype" INTEGER,
"fxmodeltime" REAL,
"inside_mul" REAL,
"inside_x" REAL,
"inside_y" REAL,
"inside_z" REAL,
"notrade" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("projectile_id") REFERENCES "Projectiles" ("id")
);

CREATE TABLE "Configurations" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"name_id" INTEGER,
"glider_id" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("glider_id") REFERENCES "Gliders" ("id")
);

CREATE TABLE "Gliders" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"standard" INTEGER,
"weight" REAL,
"maxweight" REAL,
"armor" REAL,
"price" INTEGER,
"restore" REAL,
"power" REAL,
"special" INTEGER,
"rotatespeed" REAL,
"resfront" REAL,
"restop" REAL,
"resside" REAL,
"stabfront" REAL,
"stabside" REAL,
"careen" REAL,
"delta_t" REAL,
"turbulence" REAL,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "Equipments" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"type" INTEGER,
"standard" INTEGER,
"weight" REAL,
"durability" REAL,
"power" REAL,
"value1" REAL,
"value2" REAL,
"value3" REAL,
"manual" INTEGER,
"price" REAL,
"notrade" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "ConfigurationEquipments" (
"configuration_id" INTEGER NOT NULL,
"equipment_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("configuration_id", "equipment_id") ,
FOREIGN KEY ("configuration_id") REFERENCES "Configurations" ("id"),
FOREIGN KEY ("equipment_id") REFERENCES "Equipments" ("id")
);

CREATE TABLE "Goods" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"resource_drop" TEXT,
"name_id" INTEGER,
"price" INTEGER,
"weight" REAL,
"notrade" INTEGER,
"type" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "ConfigurationGoods" (
"configuration_id" INTEGER NOT NULL,
"good_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("configuration_id", "good_id") ,
FOREIGN KEY ("configuration_id") REFERENCES "Configurations" ("id"),
FOREIGN KEY ("good_id") REFERENCES "Goods" ("id")
);

CREATE TABLE "Projectiles" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"type" INTEGER,
"weight" REAL,
"damage" REAL,
"T" TEXT,
"speed" REAL,
"scale" REAL,
"numstate" INTEGER,
"rotate" REAL,
"subtype" INTEGER,
"life_time" REAL,
"detonation_delay" REAL,
"distance_detonation" REAL,
"strength" REAL,
"price" REAL,
"notrade" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "ConfigurationProjectiles" (
"configuration_id" INTEGER NOT NULL,
"projectile_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("configuration_id", "projectile_id") ,
FOREIGN KEY ("configuration_id") REFERENCES "Configurations" ("id"),
FOREIGN KEY ("projectile_id") REFERENCES "Projectiles" ("id")
);

CREATE TABLE "Mechanoids" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"name_id" INTEGER,
"generation" INTEGER,
"rating" REAL,
"money" REAL,
"configuration_id" INTEGER,
"group_id" INTEGER,
"clan_id" INTEGER,
"rating_fight" REAL,
"rating_courier" REAL,
"rating_trade" REAL,
"map_id" INTEGER,
"mapBuilding_id" INTEGER,
"x" REAL,
"y" REAL,
"z" REAL,
"roll" REAL,
"pitch" REAL,
"yaw" REAL,
PRIMARY KEY ("id") ,
FOREIGN KEY ("configuration_id") REFERENCES "Configurations" ("id"),
FOREIGN KEY ("clan_id") REFERENCES "Clans" ("id"),
FOREIGN KEY ("map_id") REFERENCES "Maps" ("id"),
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("group_id") REFERENCES "Groups" ("id")
);

CREATE TABLE "Clans" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"member_name_id" INTEGER,
"bonusexp" INTEGER,
"bonusrepair" INTEGER,
"bonustrade" INTEGER,
"helpness" INTEGER,
"Volatile" REAL,
"noblivion" REAL,
"playereffect" REAL,
"color" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("member_name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "ClanReputations" (
"clan_id" INTEGER NOT NULL,
"clan_id2" INTEGER NOT NULL,
"reputation" REAL,
PRIMARY KEY ("clan_id", "clan_id2") ,
FOREIGN KEY ("clan_id") REFERENCES "Clans" ("id"),
FOREIGN KEY ("clan_id2") REFERENCES "Clans" ("id")
);

CREATE TABLE "Groups" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"name_id" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "Quests" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"name_id" INTEGER,
"title_id" INTEGER,
"description_id" INTEGER,
"time" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("title_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("description_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "QuestRewards" (
"id" INTEGER NOT NULL,
"quest_id" INTEGER,
"text_id" TEXT,
"money" INTEGER,
"rating" REAL,
PRIMARY KEY ("id") ,
FOREIGN KEY ("quest_id") REFERENCES "Quests" ("id")
);

CREATE TABLE "QuestRewardWeapons" (
"questReward_id" INTEGER NOT NULL,
"weapon_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "weapon_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("weapon_id") REFERENCES "Weapons" ("id")
);

CREATE TABLE "QuestRewardGliders" (
"questReward_id" INTEGER NOT NULL,
"glider_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "glider_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("glider_id") REFERENCES "Gliders" ("id")
);

CREATE TABLE "QuestRewardEquipments" (
"questReward_id" INTEGER NOT NULL,
"equipment_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "equipment_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("equipment_id") REFERENCES "Equipments" ("id")
);

CREATE TABLE "QuestRewardProjectiles" (
"questReward_id" INTEGER NOT NULL,
"projectile_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "projectile_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("projectile_id") REFERENCES "Projectiles" ("id")
);

CREATE TABLE "ConfigurationWeapons" (
"configuration_id" INTEGER NOT NULL,
"weapon_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("configuration_id", "weapon_id") ,
FOREIGN KEY ("configuration_id") REFERENCES "Configurations" ("id"),
FOREIGN KEY ("weapon_id") REFERENCES "Weapons" ("id")
);

CREATE TABLE "QuestRewardGoods" (
"questReward_id" INTEGER NOT NULL,
"good_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "good_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("good_id") REFERENCES "Goods" ("id")
);

CREATE TABLE "Buildings" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"scale" REAL DEFAULT 1,
"scale_x" REAL DEFAULT 1,
"scale_y" REAL DEFAULT 1,
"scale_z" REAL DEFAULT 1,
PRIMARY KEY ("id") 
);

CREATE TABLE "MapBuildings" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"map_id" INTEGER NOT NULL,
"building_id" INTEGER NOT NULL,
"name_id" INTEGER,
"interactive" INTEGER,
"x" REAL,
"y" REAL,
"z" REAL,
"roll" REAL,
"pitch" REAL,
"yaw" REAL,
"scale" REAL DEFAULT 1,
"scale_x" REAL DEFAULT 1,
"scale_y" REAL DEFAULT 1,
"scale_z" REAL DEFAULT 1,
PRIMARY KEY ("id") ,
FOREIGN KEY ("map_id") REFERENCES "Maps" ("id"),
FOREIGN KEY ("building_id") REFERENCES "Buildings" ("id"),
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "MapBuildingEquipments" (
"mapBuilding_id" INTEGER NOT NULL,
"equipment_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "equipment_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("equipment_id") REFERENCES "Equipments" ("id")
);

CREATE TABLE "MapBuildingGoods" (
"mapBuilding_id" INTEGER NOT NULL,
"good_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "good_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("good_id") REFERENCES "Goods" ("id")
);

CREATE TABLE "MapBuildingProjectiles" (
"mapBuilding_id" INTEGER NOT NULL,
"projectile_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "projectile_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("projectile_id") REFERENCES "Projectiles" ("id")
);

CREATE TABLE "MapBuildingGliders" (
"mapBuilding_id" INTEGER NOT NULL,
"glider_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "glider_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("glider_id") REFERENCES "Gliders" ("id")
);

CREATE TABLE "MapBuildingWeapons" (
"mapBuilding_id" INTEGER NOT NULL,
"weapon_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "weapon_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("weapon_id") REFERENCES "Weapons" ("id")
);

CREATE TABLE "Modifications" (
"id" INTEGER NOT NULL,
"name_id" INTEGER,
"directory" TEXT NOT NULL,
"author" TEXT,
"date_created" TEXT,
"date_modified" TEXT,
"comment" TEXT,
"version" TEXT,
"script_language" TEXT,
"script_main" TEXT,
"player_mechanoid_id" INTEGER,
"cooperative_player_configuration_id" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id"),
FOREIGN KEY ("player_mechanoid_id") REFERENCES "Mechanoids" ("id"),
FOREIGN KEY ("cooperative_player_configuration_id") REFERENCES "Configurations" ("id")
);

CREATE TABLE "ModificationMaps" (
"modification_id" INTEGER NOT NULL,
"map_id" INTEGER NOT NULL,
PRIMARY KEY ("modification_id", "map_id") ,
FOREIGN KEY ("modification_id") REFERENCES "Modifications" ("id"),
FOREIGN KEY ("map_id") REFERENCES "Maps" ("id")
);

CREATE TABLE "Modificators" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"probability" REAL,
"price" REAL,
"k_price" REAL,
"k_param1" REAL,
"k_param2" REAL,
"unicum_id" INTEGER,
"mask" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "MapBuildingModificators" (
"mapBuilding_id" INTEGER NOT NULL,
"modificator_id" INTEGER NOT NULL,
"quantity" INTEGER,
PRIMARY KEY ("mapBuilding_id", "modificator_id") ,
FOREIGN KEY ("mapBuilding_id") REFERENCES "MapBuildings" ("id"),
FOREIGN KEY ("modificator_id") REFERENCES "Modificators" ("id")
);

CREATE TABLE "QuestRewardReputations" (
"questReward_id" INTEGER NOT NULL,
"clan_id" INTEGER NOT NULL,
"reputation" REAL DEFAULT 1,
PRIMARY KEY ("questReward_id", "clan_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("clan_id") REFERENCES "Clans" ("id")
);

CREATE TABLE "QuestRewardModificators" (
"questReward_id" INTEGER NOT NULL,
"modificator_id" INTEGER NOT NULL,
"quantity" INTEGER DEFAULT 1,
PRIMARY KEY ("questReward_id", "modificator_id") ,
FOREIGN KEY ("questReward_id") REFERENCES "QuestRewards" ("id"),
FOREIGN KEY ("modificator_id") REFERENCES "Modificators" ("id")
);

CREATE TABLE "Players" (
"id" INTEGER NOT NULL,
"mechanoid_id" INTEGER,
PRIMARY KEY ("id") ,
FOREIGN KEY ("mechanoid_id") REFERENCES "Mechanoids" ("id")
);

CREATE TABLE "MapObjects" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"map_id" INTEGER NOT NULL,
"object_id" INTEGER NOT NULL,
"x" REAL,
"y" REAL,
"z" REAL,
"roll" REAL,
"pitch" REAL,
"yaw" REAL,
"scale" REAL DEFAULT 1,
"scale_x" REAL DEFAULT 1,
"scale_y" REAL DEFAULT 1,
"scale_z" REAL DEFAULT 1,
PRIMARY KEY ("id") ,
FOREIGN KEY ("map_id") REFERENCES "Maps" ("id"),
FOREIGN KEY ("object_id") REFERENCES "Objects" ("id")
);

CREATE TABLE "Objects" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"resource" TEXT,
"name_id" INTEGER,
"type" INTEGER,
"scale" REAL DEFAULT 1,
"scale_x" REAL DEFAULT 1,
"scale_y" REAL DEFAULT 1,
"scale_z" REAL DEFAULT 1,
PRIMARY KEY ("id") ,
FOREIGN KEY ("name_id") REFERENCES "Strings" ("id")
);

CREATE TABLE "ModificationMechanoids" (
"modification_id" INTEGER NOT NULL,
"mechanoid_id" INTEGER NOT NULL,
PRIMARY KEY ("modification_id", "mechanoid_id") ,
FOREIGN KEY ("modification_id") REFERENCES "Modifications" ("id"),
FOREIGN KEY ("mechanoid_id") REFERENCES "Mechanoids" ("id")
);

CREATE TABLE "ModificationClans" (
"modification_id" INTEGER NOT NULL,
"clan_id" INTEGER NOT NULL,
PRIMARY KEY ("modification_id", "clan_id") ,
FOREIGN KEY ("modification_id") REFERENCES "Modifications" ("id"),
FOREIGN KEY ("clan_id") REFERENCES "Clans" ("id")
);

CREATE TABLE "ScriptVariables" (
"id" INTEGER NOT NULL,
"variable" TEXT,
"value_int" INTEGER,
"value_float" REAL,
"value_text" TEXT,
PRIMARY KEY ("id") 
);

CREATE TABLE "MechanoidQuests" (
"mechanoid_id" INTEGER NOT NULL,
"quest_id" INTEGER NOT NULL,
"state" INTEGER,
PRIMARY KEY ("mechanoid_id", "quest_id") ,
FOREIGN KEY ("mechanoid_id") REFERENCES "Mechanoids" ("id"),
FOREIGN KEY ("quest_id") REFERENCES "Quests" ("id")
);

CREATE TABLE "Settings" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
"player_id" INTEGER NOT NULL,
"value_int" INTEGER,
"value_float" REAL,
"value_text" TEXT,
"value_blob" BLOB,
PRIMARY KEY ("id") ,
FOREIGN KEY ("player_id") REFERENCES "Players" ("id")
);

CREATE TABLE "GroupMechanoids" (
"group_id" INTEGER NOT NULL,
"mechanoid_id" INTEGER NOT NULL,
PRIMARY KEY ("mechanoid_id") ,
FOREIGN KEY ("group_id") REFERENCES "Groups" ("id"),
FOREIGN KEY ("mechanoid_id") REFERENCES "Mechanoids" ("id")
);

CREATE TABLE "ClanMechanoids" (
"clan_id" INTEGER NOT NULL,
"mechanoid_id" INTEGER NOT NULL,
PRIMARY KEY ("mechanoid_id") ,
FOREIGN KEY ("clan_id") REFERENCES "Clans" ("id"),
FOREIGN KEY ("mechanoid_id") REFERENCES "Mechanoids" ("id")
);

CREATE TABLE "MapGoods" (
"id" INTEGER NOT NULL,
"text_id" INTEGER,
"map_id" INTEGER,
"good_id" INTEGER,
"x" REAL,
"y" REAL,
"z" REAL,
"roll" REAL,
"pitch" REAL,
"yaw" REAL,
"scale" REAL,
"scale_x" REAL,
"scale_y" REAL,
"scale_z" REAL,
PRIMARY KEY ("id") ,
FOREIGN KEY ("map_id") REFERENCES "Maps" ("id"),
FOREIGN KEY ("good_id") REFERENCES "Goods" ("id")
);

CREATE TABLE "Tables" (
"id" INTEGER NOT NULL,
"text_id" TEXT,
PRIMARY KEY ("id") 
);

