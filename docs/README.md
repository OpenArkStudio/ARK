# ark

ark is a C++20 framework for distributed applications. Applications assemble plugins; plugins register modules that provide business and infrastructure interfaces.

- **Composition:** applications, plugins and interface-based modules.
- **Networking:** TCP, WebSocket, HTTP and RDP through the independent [arknet library](https://openarkstudio.github.io/arknet/).
- **Development:** CMake builds, permanent C++ tests and cross-platform CI.

<Aside type="note" title="Choose the right layer">
Use ark to compose application services. Use arknet directly when you need a networking library. IO execution belongs to arknet; ark dispatches business callbacks through its `update()` loop.
</Aside>
