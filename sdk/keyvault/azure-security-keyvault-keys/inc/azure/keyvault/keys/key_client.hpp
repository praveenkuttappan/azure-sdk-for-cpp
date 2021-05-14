// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

/**
 * @file
 * @brief Defines the Key Vault Keys client.
 *
 */

#pragma once

#include <azure/keyvault/common/internal/keyvault_pipeline.hpp>

#include "azure/keyvault/keys/delete_key_operation.hpp"
#include "azure/keyvault/keys/import_key_options.hpp"
#include "azure/keyvault/keys/key_client_options.hpp"
#include "azure/keyvault/keys/key_create_options.hpp"
#include "azure/keyvault/keys/key_type.hpp"
#include "azure/keyvault/keys/key_vault_key.hpp"
#include "azure/keyvault/keys/list_keys_single_page_result.hpp"
#include "azure/keyvault/keys/recover_deleted_key_operation.hpp"

#include <functional>
#include <list>
#include <vector>

namespace Azure { namespace Security { namespace KeyVault { namespace Keys {

  /**
   * @brief Define a model for a purged key.
   *
   */
  struct PurgedKey final
  {
  };

  /**
   * @brief Optional parameters for KeyVaultClient::GetKey
   *
   */
  struct GetKeyOptions final
  {
    /**
     * @brief Specify the key version to get.
     *
     */
    std::string Version;
  };

  /**
   * @brief The KeyClient provides synchronous methods to manage a KeyVaultKe in the Azure Key
   * Vault. The client supports creating, retrieving, updating, deleting, purging, backing up,
   * restoring, and listing the KeyVaultKey.
   */
  class KeyClient
#if !defined(TESTING_BUILD)
      final
#endif
  {
  protected:
    // Using a shared pipeline for a client to share it with LRO (like delete key)
    std::shared_ptr<Azure::Security::KeyVault::_internal::KeyVaultPipeline> m_pipeline;

  public:
    /**
     * @brief Destructor.
     *
     */
    virtual ~KeyClient() = default;

    /**
     * @brief Construct a new Key Client object
     *
     * @param vaultUrl The url address where the client will send the requests to.
     * @param credential The authentication method to use.
     * @param options The options to customize the client behavior.
     */
    explicit KeyClient(
        std::string const& vaultUrl,
        std::shared_ptr<Core::Credentials::TokenCredential const> credential,
        KeyClientOptions options = KeyClientOptions());

    /**
     * @brief Construct a new Key Client object from another key client.
     *
     * @param keyClient An existing key vault key client.
     */
    explicit KeyClient(KeyClient const& keyClient) : m_pipeline(keyClient.m_pipeline) {}

    /**
     * @brief Gets the public part of a stored key.
     *
     * @remark The get key operation is applicable to all key types. If the requested key is
     * symmetric, then no key is released in the response. This operation requires the keys/get
     * permission.
     *
     * @param name The name of the key.
     * @param options Optional parameters for this operation.
     * @param context The context for the operation can be used for request cancellation.
     * @return The Key wrapped in the Response.
     */
    Azure::Response<KeyVaultKey> GetKey(
        std::string const& name,
        GetKeyOptions const& options = GetKeyOptions(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Creates and stores a new key in Key Vault. The create key operation can be used to
     * create any key type in Azure Key Vault. If the named key already exists, Azure Key Vault
     * creates a new version of the key. It requires the keys/create permission.
     *
     * @param name The name of the key.
     * @param keyType The type of key to create. See
     * #Azure::Security::KeyVault::Keys::KeyVaultKeyType.
     * @param options Optional parameters for this operation. See
     * #Azure::Security::KeyVault::Keys::CreateKeyOptions.
     * @param context The context for the operation can be used for request cancellation.
     * @return The Key wrapped in the Response.
     */
    Azure::Response<KeyVaultKey> CreateKey(
        std::string const& name,
        KeyVaultKeyType keyType,
        CreateKeyOptions const& options = CreateKeyOptions(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Creates and stores a new Elliptic Curve key in Key Vault.
     *
     * @remark If the named key already exists, Azure Key Vault creates a new version of the key.
     *
     * @remark This operation requires the keys/create permission.
     *
     * @param ecKeyOptions The key options object containing information about the Elliptic Curve
     * key being created.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return The Key wrapped in the Response.
     */
    Azure::Response<KeyVaultKey> CreateEcKey(
        CreateEcKeyOptions const& ecKeyOptions,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Creates and stores a new RSA key in Key Vault.
     *
     * @remark If the named key already exists, Azure Key Vault creates a new version of the key.
     *
     * @remark This operation requires the keys/create permission.
     *
     * @param rsaKeyOptions The key options object containing information about the RSA key being
     * created.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return The Key wrapped in the Response.
     */
    Azure::Response<KeyVaultKey> CreateRsaKey(
        CreateRsaKeyOptions const& rsaKeyOptions,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Creates and stores a new AES key in Key Vault.
     *
     * @remark If the named key already exists, Azure Key Vault creates a new version of the key.
     *
     * @remark This operation requires the keys/create permission.
     *
     * @param octKeyOptions The key options object containing information about the AES key being
     * created.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return The Key wrapped in the Response.
     */
    Azure::Response<KeyVaultKey> CreateOctKey(
        CreateOctKeyOptions const& octKeyOptions,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Get a single page with the properties of all keys in the specified vault. You can use
     * the returned #KeyProperties.Name in subsequent calls to #GetKey.
     *
     * @remark Retrieves a list of the keys in the Key Vault that contains the public part of a
     * stored key. The operation is applicable to all key types, however only the base key
     * identifier, attributes, and tags are provided in the response. Individual versions of a key
     * are not listed in the response. This operation requires the keys/list permission.
     *
     * @remark Use \p options to control which page to get. If
     * #GetPropertiesOfKeysOptions.NextLink is not set, the operation will get the first
     * page and it will set the `NextPageToken` from the #KeyPropertiesPageResult as the next
     * page of the response if there is a next page.
     *
     * @param options The #GetPropertiesOfKeysOptions object to for setting the operation
     * up.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    KeyPropertiesPageResult GetPropertiesOfKeys(
        GetPropertiesOfKeysOptions const& options = GetPropertiesOfKeysOptions(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Get one page listing the properties of all the versions of the specified key. You can
     * use the returned #KeyProperties.Name and #KeyProperties.Version in subsequent calls to
     * #GetKey.
     *
     * @remark The full identifier, attributes, and tags are provided in the response. This
     * operation requires the keys/list permission.
     *
     * @remark Use \p options to control which page to get. If
     * #GetPropertiesOfKeyVersionsOptions.NextLink is not set, the operation will get the
     * first page and it will set the `NextPageToken` from the #KeyPropertiesPageResult as the
     * next page of the response if there is a next page.
     *
     * @param name The name of the key.
     * @param options The #GetPropertiesOfKeyVersionsOptions object to for setting the
     * operation up.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    KeyPropertiesPageResult GetPropertiesOfKeyVersions(
        std::string const& name,
        GetPropertiesOfKeyVersionsOptions const& options = GetPropertiesOfKeyVersionsOptions(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Deletes a key of any type from storage in Azure Key Vault.
     *
     * @remark The delete key operation cannot be used to remove individual versions of a key. This
     * operation removes the cryptographic material associated with the key, which means the key is
     * not usable for Sign/Verify, WrapKey/Unwrap or Encrypt/Decrypt operations. This operation
     * requires the keys/delete permission.
     *
     * @param name The name of the key.
     * @param context A cancellation token controlling the request lifetime.
     * @return A #Azure::Security::KeyVault::Keys::DeleteKeyOperation to wait on this long-running
     * operation. If the key is soft delete-enabled, you only need to wait for the operation to
     * complete if you need to recover or purge the key; otherwise, the key is deleted automatically
     * on purge schedule.
     */
    Azure::Security::KeyVault::Keys::DeleteKeyOperation StartDeleteKey(
        std::string const& name,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Gets the public part of a deleted key.
     *
     * @remark The Get Deleted Key operation is applicable for soft-delete enabled vaults. While the
     * operation can be invoked on any vault, it will return an error if invoked on a non
     * soft-delete enabled vault. This operation requires the keys/get permission.
     *
     * @param name The name of the key.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return Azure::Response<DeletedKey>
     */
    Azure::Response<DeletedKey> GetDeletedKey(
        std::string const& name,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Get a page listing the deleted keys in the specified vault.
     *
     * @remark Retrieves the list of the keys in the Key Vault that contains the public part of the
     * deleted key. This operation includes deletion-specific information. This operation is
     * applicable for vaults enabled for soft-delete. While the operation can be invoked on any
     * vault, it will return error if invoked on a non soft-delete enabled vault. This operation
     * requires the keys/list permission.
     *
     * @remark Use \p options to control which page to get. If
     * #GetPropertiesOfKeyVersionsOptions.NextPageToken is not set, the operation will get
     * the first page and it will set the `NextPageToken` from the #KeyPropertiesPageResult as the
     * next page of the response if there is a next page.
     *
     * @param options The #GetDeletedKeysOptions object to for setting the operation up.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return Azure::Response<DeletedKeyPageResult>
     */
    DeletedKeyPageResult GetDeletedKeys(
        GetDeletedKeysOptions const& options = GetDeletedKeysOptions(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Permanently deletes the specified key.
     *
     * @remark The Purge Deleted Key operation is applicable for soft-delete enabled values. While
     * the operation can be invoked on any vault, it will return an error if invoked on a non
     * soft-delete enabled vault. This operation requires the keys/purge permission.
     *
     * @param name The name of the key.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    Azure::Response<PurgedKey> PurgeDeletedKey(
        std::string const& name,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Recovers the deleted key to its latest version.
     *
     * @remark The Recover Deleted Key operation is applicable for deleted keys in soft-delete
     * enabled vaults. It recovers the deleted key back to its latest version under /keys. An
     * attempt to recover an non-deleted key will return an error. Consider this the inverse of the
     * delete operation on soft-delete enabled vaults. This operation requires the keys/recover
     * permission.
     *
     * @param name The name of the key.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return Azure::Security::KeyVault::Keys::RecoverDeletedKeyOperation
     */
    Azure::Security::KeyVault::Keys::RecoverDeletedKeyOperation StartRecoverDeletedKey(
        std::string const& name,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief  The update key operation changes specified attributes of a stored key and can be
     * applied to any key type and key version stored in Azure Key Vault.
     *
     * @remark In order to perform this operation, the key must already exist in the Key Vault.
     * Note: The cryptographic material of a key itself cannot be changed. This operation requires
     * the keys/update permission.
     *
     * @param properties The #KeyProperties object with updated properties.
     * @param keyOperations Optional list of supported #KeyOperation. If no operation list provided,
     * no changes will be made to existing key operations.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return Azure::Response<KeyVaultKey>
     */
    Azure::Response<KeyVaultKey> UpdateKeyProperties(
        KeyProperties const& properties,
        Azure::Nullable<std::list<KeyOperation>> const& keyOperations
        = Azure::Nullable<std::list<KeyOperation>>(),
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Request that a backup of the specified be downloaded to the client.
     *
     * @remark The Key Backup operation exports a key from Azure Key Vault in a protected form. Note
     * that this operation does NOT return the actual key in a form that can be used outside the
     * Azure Key Vault system, the returned key is either protected to a Azure Key Vault HSM or to
     * Azure Key Vault itself. The intent of this operation is to allow a client to GENERATE a key
     * in one Azure Key Vault instance, BACKUP the key, and then RESTORE it into another Azure Key
     * Vault instance. The BACKUP operation may be used to export, in protected form, any key type
     * from Azure Key Vault. Individual versions of a key cannot be backed up. BACKUP / RESTORE can
     * be performed within geographical boundaries only; meaning that a BACKUP from one geographical
     * are cannot be restored to another geographical are. For example, a backup from the US
     * geographical are cannot be restored in an EU geographical area. This operation requires the
     * key/backup permission.
     *
     * @param name The name of the key.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    Azure::Response<std::vector<uint8_t>> BackupKey(
        std::string const& name,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Restores a backed up key to keyvault.
     *
     * @remark Imports a previously backed up key into Azure Key Vault, restoring the key, its key
     * identifier, attributes, and access control policies. The RESTORE operation may be used to
     * import a previously backed up key. Individual versions of a key cannot be restored. The key
     * is restored in its entirety with the same key name as it had when it was backed up. If the
     * key name is not available in the target Key Vault, the RESTORE operation will be rejected.
     * While the key name is retained during restore, the final key identifier will change if the
     * key is restored to a different vault. Restore will restore all versions and preserve version
     * identifiers. The RESTORE operation is subject to security constrains: The target Key Vault
     * must be owned by the same Microsoft Azure Subscription as the source Key Vault. The user must
     * have RESTORE permission in the target Key Vault. This operation requires the keys/restore
     * permission.
     *
     * @param backup The backup blob associated with a key.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    Azure::Response<KeyVaultKey> RestoreKeyBackup(
        std::vector<uint8_t> const& backup,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Imports an externally created ket, stores it, and returns jey parameters and
     * attributes to the client.
     *
     * @remark The import key operation may be used to import any key type into an Azure Key Vault.
     * If the named key already exists, Azure Key Vault creates a new version of the key. This
     * operation requires the keys/import permission.
     *
     * @param name The name of the key.
     * @param keyMaterial The #JsonWebKey being imported.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     * @return Azure::Response<KeyVaultKey>
     */
    Azure::Response<KeyVaultKey> ImportKey(
        std::string const& name,
        JsonWebKey const& keyMaterial,
        Azure::Core::Context const& context = Azure::Core::Context()) const;

    /**
     * @brief Imports an externally created key, stores it, and returns key parameters and
     * attributes to the client.
     *
     * @remark The import operation may be used to import any key type into an Azure Key Vault. If
     * the named key already exists, Azure Key Vault creates a new version of the key. This
     * operation requires the keys/import permission.
     *
     * @param importKeyOptions The key import configuration object containing information about the
     * #JsonWebKey being imported.
     * @param context A #Azure::Core::Context controlling the request lifetime.
     */
    Azure::Response<KeyVaultKey> ImportKey(
        ImportKeyOptions const& importKeyOptions,
        Azure::Core::Context const& context = Azure::Core::Context()) const;
  };
}}}} // namespace Azure::Security::KeyVault::Keys
