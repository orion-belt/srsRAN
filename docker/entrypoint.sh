#!/usr/bin/env bash

set -euo pipefail

CONFIG_DIR="/root/.config/srsran"
RR_TAC=`echo $(($TAC))`

array=(${CONFIG_DIR}/enb.conf ${CONFIG_DIR}/rr.conf ${CONFIG_DIR}/ue.conf)

for c in "${array[@]}"; do
    # grep variable names (format: ${VAR}) from template to be rendered
    VARS=$(grep -oP '@[a-zA-Z0-9_]+@' ${c} | sort | uniq | xargs)
    echo "Now setting these variables '${VARS}'"

    # create sed expressions for substituting each occurrence of ${VAR}
    # with the value of the environment variable "VAR"
    EXPRESSIONS=""
    for v in ${VARS}; do
        NEW_VAR=$(echo $v | sed -e "s#@##g")
        if [[ -z ${!NEW_VAR+x} ]]; then
            echo "Error: Environment variable '${NEW_VAR}' is not set." \
                "Config file '$(basename $c)' requires all of $VARS."
            exit 1
        fi
        EXPRESSIONS="${EXPRESSIONS};s|${v}|${!NEW_VAR}|g"
    done
    EXPRESSIONS="${EXPRESSIONS#';'}"

    # render template and inline replace config file
    sed -i "${EXPRESSIONS}" ${c}
done

echo -e  "Done setting the configuration\n"

echo -e  "Running gNB Service \n"
./srsenb &

sleep 5

if [[ ${RUN_ZMQ_UE} == "yes" ]];then
        echo -e  "Running UE Service \n"
        ip netns add ue1
        ./srsue
fi

exec "$@"

