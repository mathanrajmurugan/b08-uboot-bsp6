#!/bin/bash

DECONFIG="seco_b08_defconfig"

if [ "x${PKG_CONFIG_SYSROOT_DIR}" != "x" ]
  then
    if [ `echo ${PKG_CONFIG_SYSROOT_DIR} | grep -c "tmp/sysroots/seco-b08"` -gt 0 ]
      then
        MAKE_OPT="ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- CC="
        CC_OPT="arm-poky-linux-gnueabi-gcc --sysroot=$PKG_CONFIG_SYSROOT_DIR"
    fi
fi 

make distclean
make O=${DECONFIG} distclean
cp ./include/configs/seco_b08_bootdevice_uSD.h ./include/configs/seco_b08_bootdevice.h
make O=${DECONFIG} ${DECONFIG}

if [ "x${MAKE_OPT}" == "x" ]
  then
    make -j 2 O=${DECONFIG}
 else
    make -j 2 ${MAKE_OPT}"${CC_OPT}" O=${DECONFIG}
fi

dd if=${DECONFIG}/SPL of=/tmp/u-boot-sd.imx bs=1K seek=0 conv=notrunc;
dd if=${DECONFIG}/u-boot.img of=/tmp/u-boot-sd.imx bs=1K seek=68; 

echo "u-boot image for uSD compiled."

sleep 1

make O=${DECONFIG} distclean
cp ./include/configs/seco_b08_bootdevice_spi.h ./include/configs/seco_b08_bootdevice.h
make O=${DECONFIG} ${DECONFIG}

if [ "x${MAKE_OPT}" == "x" ]
  then
    make -j 2 O=${DECONFIG}
 else
    make -j 2 ${MAKE_OPT}"${CC_OPT}" O=${DECONFIG}
fi

dd if=${DECONFIG}/SPL of=u-boot.imx bs=1K seek=0 conv=notrunc;
dd if=${DECONFIG}/u-boot.img of=u-boot.imx bs=1K seek=68; 
dd if=qspic_header.imx of=${DECONFIG}/u-boot.spi seek=0 conv=notrunc
./crea_qspi_image.py
dd if=footer.spi of=${DECONFIG}/u-boot.spi bs=1k seek=3

echo "u-boot image for SPI flash compiled."

cp ${DECONFIG}/u-boot.spi u-boot.spi
cp /tmp/u-boot-sd.imx ${DECONFIG}/u-boot.imx
mv /tmp/u-boot-sd.imx u-boot.imx

echo ""
echo ""
echo ""
echo -e "\tProduced \"u-boot.imx\" for uSD card,\n\tand \"u-boot.spi\" for spi flash." 
echo ""
echo ""
