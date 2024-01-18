This document describes PubSub as supported by Vanity.

PubSub is a mechanism to subscribe to events, and receive notifications when those events occur.  

When a message is published to a channel, all clients subscribed to that channel will receive an `ASYNC`  response with a `:PUBLISH` annotation, that looks like this:

- `ASYNC :PUBLISH<channel><message>`
  where
    - `<channel>` is the channel the message was published to, a `<string>`.
    - `<message>` is the message that was published, a `<string>`.


PubSub is supported by the following commands:

- `PUBLISH<channel><message>`
    Publishes a message to a channel.  
    Returns an `OK` response.

    `<channel>` is the channel to publish to, a `<string>`.  
    `<message>` is the message to publish, a `<string>`.

    Example:
    ```
    PUBLISH (7)channel (5)hello
    OK
    ```

- `SUBSCRIBE<channel>`
  Subscribes to a channel.  
  Returns an `OK` response, with a value containing the number of channels currently subscribed to.  

  `<channel>` is the channel to subscribe to, a `<string>`.

  Example:
  ```
  SUBSCRIBE (7)channel
  OK :INT 1
  ```

- `SUBSCRIBED`
  Returns an array of channels currently subscribed to.

  Example:
  ```
  SUBSCRIBED
  OK :ARR (1)[(7)channel]
  ```
  

- `UNSUBSCRIBE<channel>`
  Unsubscribes from a channel.  
  Returns an `OK` response, with a value containing the number of channels currently subscribed to.  

  `<channel>` is the channel to unsubscribe from, a `<string>`.

  Example:
  ```
  UNSUBSCRIBE (7)channel
  OK :INT 0
  ```

- `UNSUBSCRIBE_ALL`
  Unsubscribes from all channels.  
  Returns an `OK` response.

  Example:
  ```
  UNSUBSCRIBE_ALL
  OK
  ``
