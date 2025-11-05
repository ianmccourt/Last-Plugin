#pragma once

#include <juce_core/juce_core.h>
#include <atomic>
#include <memory>

/**
 * Lock-free, thread-safe queue for communication between audio and UI threads
 * Uses atomic operations and memory ordering for high-performance communication
 */
template<typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() : head(new Node), tail(head.load()) {}
    
    ~ThreadSafeQueue()
    {
        // Clean up remaining nodes
        while (Node* const oldHead = head.load())
        {
            head.store(oldHead->next);
            delete oldHead;
        }
    }
    
    // Producer side (typically UI thread)
    void push(T item)
    {
        auto data = std::make_unique<T>(std::move(item));
        Node* const newNode = new Node;
        Node* const prevTail = tail.exchange(newNode);
        prevTail->data = std::move(data);
        prevTail->next.store(newNode);
    }
    
    // Consumer side (typically audio thread or message thread)
    bool tryPop(T& result)
    {
        Node* oldHead = head.load();
        if (oldHead == tail.load())
        {
            return false; // Queue is empty
        }
        
        if (oldHead->data)
        {
            result = std::move(*oldHead->data);
            head.store(oldHead->next);
            delete oldHead;
            return true;
        }
        
        return false;
    }
    
    // Check if queue is empty (approximate - may change immediately after call)
    bool empty() const
    {
        return head.load() == tail.load();
    }
    
    // Get approximate size (for monitoring/debugging)
    size_t approximateSize() const
    {
        size_t count = 0;
        Node* current = head.load();
        Node* tailNode = tail.load();
        
        while (current != tailNode && count < 1000) // Prevent infinite loops
        {
            if (current->data)
                ++count;
            current = current->next.load();
            if (!current) break;
        }
        
        return count;
    }

private:
    struct Node
    {
        std::atomic<Node*> next{nullptr};
        std::unique_ptr<T> data;
    };
    
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreadSafeQueue)
};

/**
 * Specialized message types for inter-thread communication
 */
namespace ThreadMessages
{
    enum class MessageType
    {
        PresetGenerated,
        PresetGenerationFailed,
        PresetGenerationStarted,
        ParameterChanged,
        StatusUpdate
    };
    
    struct BaseMessage
    {
        MessageType type;
        juce::Time timestamp;
        
        BaseMessage(MessageType t) : type(t), timestamp(juce::Time::getCurrentTime()) {}
        virtual ~BaseMessage() = default;
    };
    
    struct PresetGeneratedMessage : public BaseMessage
    {
        PresetData preset;
        juce::String originalPrompt;
        int generationTimeMs;
        
        PresetGeneratedMessage(PresetData p, const juce::String& prompt, int timeMs)
            : BaseMessage(MessageType::PresetGenerated)
            , preset(std::move(p))
            , originalPrompt(prompt)
            , generationTimeMs(timeMs)
        {}
    };
    
    struct PresetGenerationFailedMessage : public BaseMessage
    {
        juce::String errorMessage;
        juce::String originalPrompt;
        
        PresetGenerationFailedMessage(const juce::String& error, const juce::String& prompt)
            : BaseMessage(MessageType::PresetGenerationFailed)
            , errorMessage(error)
            , originalPrompt(prompt)
        {}
    };
    
    struct PresetGenerationStartedMessage : public BaseMessage
    {
        juce::String prompt;
        
        PresetGenerationStartedMessage(const juce::String& p)
            : BaseMessage(MessageType::PresetGenerationStarted)
            , prompt(p)
        {}
    };
    
    struct ParameterChangedMessage : public BaseMessage
    {
        juce::String parameterId;
        float newValue;
        bool isSmoothed;
        
        ParameterChangedMessage(const juce::String& id, float value, bool smoothed = true)
            : BaseMessage(MessageType::ParameterChanged)
            , parameterId(id)
            , newValue(value)
            , isSmoothed(smoothed)
        {}
    };
    
    struct StatusUpdateMessage : public BaseMessage
    {
        juce::String statusText;
        float progress; // 0.0 to 1.0, -1.0 for indeterminate
        
        StatusUpdateMessage(const juce::String& status, float prog = -1.0f)
            : BaseMessage(MessageType::StatusUpdate)
            , statusText(status)
            , progress(prog)
        {}
    };
}

using MessageQueue = ThreadSafeQueue<std::unique_ptr<ThreadMessages::BaseMessage>>;
