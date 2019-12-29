# This file can be used to uninstall old roccat installations if no package
# management system was used and the build files have been deleted.
# Please delete the next two lines and edit the following two lines before running.

echo "Please edit me first..."
exit

INSTALL_PREFIX=/usr
LIBDIR=${INSTALL_PREFIX}/lib64
UDEVDIR=/lib/udev/rules.d

RM_COMMAND="rm -i"
RMDIR_COMMAND="rmdir"

for device in arvo isku iskufx kone koneplus konepure konepuremilitary konepureoptical konextd konextdoptical kovaplus lua pyra ryos ryosmk ryostkl savu tyon; do
	${RM_COMMAND} ${LIBDIR}/roccat/lib${device}.so*
	${RM_COMMAND} ${LIBDIR}/roccat/lib${device}eventhandler.so*
	${RM_COMMAND} ${UDEVDIR}/90-roccat-${device}.rules
	${RM_COMMAND} ${INSTALL_PREFIX}/share/applications/${device}config.desktop
	${RM_COMMAND} ${INSTALL_PREFIX}/share/man/*/man1/${device}control.1
	${RM_COMMAND} ${INSTALL_PREFIX}/bin/${device}control
	${RM_COMMAND} ${INSTALL_PREFIX}/bin/${device}config
done

${RM_COMMAND} ${LIBDIR}/roccat/libroccat.so*
${RM_COMMAND} ${LIBDIR}/roccat/libroccatwidget.so*
${RM_COMMAND} ${INSTALL_PREFIX}/bin/roccateventhandler
${RM_COMMAND} ${INSTALL_PREFIX}/share/applications/roccateventhandler.desktop
${RM_COMMAND} ${INSTALL_PREFIX}/share/icons/hicolor/*/apps/roccat.png
${RM_COMMAND} /etc/ld.so.conf.d/roccat.conf

${RMDIR_COMMAND} ${INSTALL_PREFIX}/share/roccat
${RMDIR_COMMAND} ${LIBDIR}/roccat
