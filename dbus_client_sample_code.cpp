/******** D-Bus client method call *********/

   /* Get a connection to default system bus */
    auto b = sdbusplus::bus::new_default_system();

    /* Create a method to be called */
    auto m = b.new_method_call("<service>", "<object-path>",
            "<interface>", "<method>");

    /* Append the arguments to the method */
    m.append("ss", str1, str2);

    /* Call the method */
    auto reply = b.call(m);

    /* Read the return status */
    int32_t retStatus;
    reply.read(retStatus);

/********** D-Bus client set-property call *********/

   /* Get a connection to default system bus */
    auto b = sdbusplus::bus::new_default_system();

    /* Create a set-property method to be called */
    auto m = b.new_method_call("<service>", "<object-path>",
            "org.freedesktop.DBus.Properties", "Set");  // service, object path, D-Bus Properties interface, “Set”

    /* Append the arguments to the method */
    m.append("ssv",”<actual interface>”, "<property-name>", “s <str1>“); // actual interface, property, “variant”

    /* Call the method */
    auto reply = b.call(m);

    /* Read the return status */
    int32_t retStatus;
    reply.read(retStatus);
