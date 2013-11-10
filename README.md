node-libspotify
===============

Investigations on a Windows-based Node.JS wrapper for libspotify.

The basic idea is to expose a set of Node APIs that allow for playing,
searching, and queuing up music to be played. The music is played using
somethign like DirectSound on the server, and mobile clients connect
to Node using a RESTful API to find music and queue it up.
