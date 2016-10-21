FROM ignf/gilviewer

ENV BUILD_PACKAGES="$BUILD_PACKAGES xsltproc doxygen"

RUN apt-get -y -qq update && apt-get -y --fix-missing install $BUILD_PACKAGES \
&& git clone https://github.com/IGNF/librjmcmc.git \
&& cd librjmcmc \
&& mkdir build \
&& cd build \
&& cmake -DCMAKE_BUILD_TYPE=Release .. \
&& make install \
&& cp -R librjmcmc/build/data  /usr/local/share/librjmcmc \
&& cp librjmcmc/build/samples/quickstart/quickstart /usr/local/bin \
&& cp librjmcmc/build/samples/building_footprint_rectangle/cli/building_footprint_rectangle_cli /usr/local/bin

CMD quickstart
