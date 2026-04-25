script_folder="/home/zb_bamboo/DEV/__NEW__/CPP/GH_update-checker/gh-update-checker/test_package/build/gcc-15-x86_64-23-release/generators"
echo "echo Restoring environment" > "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
for v in OPENSSL_MODULES
do
   is_defined="true"
   value=$(printenv $v) || is_defined="" || true
   if [ -n "$value" ] || [ -n "$is_defined" ]
   then
       echo export "$v='$value'" >> "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
   else
       echo unset $v >> "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
   fi
done

export OPENSSL_MODULES="/home/zb_bamboo/.conan2/p/b/opens53724d897cb7d/p/lib/ossl-modules"