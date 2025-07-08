//
// Created by leipeng on 2025/7/8
//
#include "rocksdb/db.h"
#include "rocksdb/utilities/backup_engine.h"
#include "rocksdb/side_plugin_factory.h"

namespace ROCKSDB_NAMESPACE {

struct BackupEngineOptionsJS : BackupEngineOptions {
  BackupEngineOptionsJS() {
    share_files_with_checksum = false; // we change the default value
  }
  void Update(const json& js, const SidePluginRepo& repo) {
    ROCKSDB_JSON_REQ_PROP(js, backup_dir);
    ROCKSDB_JSON_OPT_FACT(js, backup_env);
    ROCKSDB_JSON_OPT_PROP(js, share_table_files);
    ROCKSDB_JSON_OPT_PROP(js, sync);
    ROCKSDB_JSON_OPT_PROP(js, destroy_old_data);
    ROCKSDB_JSON_OPT_PROP(js, backup_log_files);
    ROCKSDB_JSON_OPT_PROP(js, backup_rate_limit);
    ROCKSDB_JSON_OPT_FACT(js, backup_rate_limiter);
    ROCKSDB_JSON_OPT_PROP(js, restore_rate_limit);
    ROCKSDB_JSON_OPT_FACT(js, restore_rate_limiter);
    ROCKSDB_JSON_OPT_PROP(js, share_files_with_checksum);
    ROCKSDB_JSON_OPT_PROP(js, max_background_operations);
    ROCKSDB_JSON_OPT_SIZE(js, callback_trigger_interval_size);
    ROCKSDB_JSON_OPT_PROP(js, max_valid_backups_to_open);
    //share_files_with_checksum_naming
    //schema_version
    //current_temperatures_override_manifest
  }
};

class BackupWebPanel : public AnyPlugin {
 public:
  const char* Name() const override { return "BackupWebPanel"; }
  void Update(const json& query, const json& body, const SidePluginRepo& repo) override {
    // This function should not be called for BackupWebPanel
    ROCKSDB_DIE("This function should not be called");
  }
  std::string ToString(const json& query, const SidePluginRepo& repo) const override {
    //bool html = JsonSmartBool(query, "html", true);
    bool html = true;
    std::string action = "list_db";
    ROCKSDB_JSON_OPT_PROP(query, action);
    if (action == "list_db") {
    } else if (action == "list_backups") {
    } else if (action == "get_backup_info") {
    } else if (action == "restore_db") {
    } else if (action == "garbage_collect") {
      djs["action"] = "garbage_collect";
    } else if (action == "create_backup") {
    } else {
      THROW_InvalidArgument("Unknown action: %s", action);
    }
    return JsonToString(djs, query);
  }

  std::string create_backup(const json& query, const SidePluginRepo& repo) {
      BackupEngineOptionsJS beo;
      std::string dbname = JsonSmartString(query, "dbname", "");
      if (dbname.empty()) {
        THROW_InvalidArgument("dbname is required for BackupWebPanel");
      }
      if (beo.backup_dir.empty()) {
        THROW_InvalidArgument("backup_dir is required for BackupWebPanel");
      }
      Status s;
      DB* db = nullptr;
      if (!s.ok()) {
        THROW_InvalidArgument("Failed to get database: '" + dbname + "' reason: " + s.ToString());
      }
      if (!repo.Get(dbname, &db, &s)) {
        THROW_InvalidArgument("database '" + dbname + "' does not exist");
      }
      std::string response;
      BackupEngine* backup_engine;
      s = BackupEngine::Open(beo, db->GetEnv(), &backup_engine);
      if (!s.ok()) {
        THROW_InvalidArgument("Failed to open backup engine: " + s.ToString());
      }
      ROCKSDB_SCOPE_EXIT(delete backup_engine);
      IOStatus ios = backup_engine->CreateNewBackup(db, true);
      if (!ios.ok()) {
        THROW_InvalidArgument("Failed to create backup: " + ios.ToString());
      }
      return response;
  }
};


} // namespace ROCKSDB_NAMESPACE
