ARG VARIANT="4"
FROM ghcr.io/rocker-org/devcontainer/geospatial:${VARIANT}

ENV DISABLE_AUTH="true"

RUN python3 -m pip install ipadic && \
    python3 -c "import ipadic; print('dicdir=' + ipadic.DICDIR);" > /usr/local/etc/.mecabrc
