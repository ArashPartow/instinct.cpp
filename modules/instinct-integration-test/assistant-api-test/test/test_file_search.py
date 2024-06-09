import os
import logging
import pytest
import json
import time
from openai import OpenAI

logging.basicConfig(level=logging.DEBUG)

client = OpenAI(base_url="http://localhost:9091/v1")
MODEL_NAME = os.getenv("MODEL_NAME", "gpt-3.5-turbo")


def test_file_search_with_vs():
    vector_store = client.beta.vector_stores.create(name="kb")

    # Use the upload and poll SDK helper to upload the files, add them to the vector store,
    # and poll the status of the file batch for completion.
    vector_store_file = client.beta.vector_stores.files.upload_and_poll(
        vector_store_id=vector_store.id,
        file=open("munsey_magazine.txt", "rb")
    )

    # You can print the status and the file counts of the batch to see the result of this operation.
    print(vector_store_file.status)
    assert vector_store_file.status == "completed"

    # create assistant
    assistant = client.beta.assistants.create(
        name="Financial Analyst Assistant",
        instructions="You are assistant researcher. Use your knowledge to answer questions about the papers.",
        model="gpt-3.5-turbo",
        tools=[{"type": "file_search"}],
        tool_resources={"file_search": {"vector_store_ids": [vector_store.id]}}
    )

    # Create a thread and attach the file to the message
    thread = client.beta.threads.create(
        messages=[
            {
                "role": "user",
                "content": "What is Munsey's Magazine?",
            }
        ]
    )
    print(thread)

    # create run
    run = client.beta.threads.runs.create_and_poll(
        thread_id=thread.id, assistant_id=assistant.id
    )
    assert run.status == "completed"

    messages = list(client.beta.threads.messages.list(thread_id=thread.id, run_id=run.id))
    message_content = messages[0].content[0].text
    print(message_content)
    annotations = message_content.annotations
    citations = []
    for index, annotation in enumerate(annotations):
        message_content.value = message_content.value.replace(annotation.text, f"[{index}]")
        if file_citation := getattr(annotation, "file_citation", None):
            cited_file = client.files.retrieve(file_citation.file_id)
            citations.append(f"[{index}] {cited_file.filename} "
                             f"({annotation.start_index} to {annotation.end_index}): {file_citation.quote}")
    print("------------------------------")
    print(message_content.value)
    print("\n".join(citations))


